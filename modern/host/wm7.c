// Host build: the ARM7's wireless manager (WM, PXI tag 10): DS local wireless between copies of the program, over UDP.
//
// On the DS the ARM9's WM library (autoload_2, include/nitro/wm.h) sends each request as a 0x100-byte message to the
// ARM7, which runs the radio and answers with callback messages: a request's own result in the request's buffer (the
// buffer is free again once the ARM9 has handled it: bit 15 of its id), and the events that follow (beacons sent, a
// child connected, MP sequences, received data) in buffers of its own. This file is that ARM7 side. What the radio
// carries goes in UDP datagrams instead: a parent's beacons to every copy on the machine and the LAN (and to the
// addresses that asked for them), the connection handshake, and the MP exchange - each MP sequence the parent sends
// one frame to its children and each child answers with the data it has set, as the DS's hardware does. The ARM9's
// layers above (the game's ov066 and ov067, the SDK's data sharing) keep their own sequence numbers, acknowledgements
// and resends, so a datagram lost or late only slows them.
//
// Addresses: every copy binds the first free UDP port of 31470-31477 (ACWW_NET_PORT: another one) and sends its
// beacons to all eight ports, on the broadcast address and on the loopback address. ACWW_NET_PEER=host[:port][,...]:
// copies elsewhere (over the internet); a child that scans asks them for their beacons, so only the parent's port has
// to be reachable. ACWW_NET_LAN=0: no beacons on the machine and the LAN, only to the peers and the copies that ask.
// The MAC address (the user settings' copy in the system work area, where the game reads it) tells the copies apart:
// ACWW_MAC=xx:xx:xx:xx:xx:xx, else one made from the machine's name and the save file's path (a fixed one for
// headless runs). ACWW_WM_LOG=1 logs the requests, the connections and the MP traffic once a second, =2 every packet.
//
// Time: the requests' results are due after a short delay of virtual time (the SDK's callers rely on a call not
// completing before it returns), scans and channel measurements after their virtual duration, MP sequences
// (mpFreq per frame) and beacons (the parent's beacon period) on virtual time; a peer is lost after 10 s of real time
// without a datagram.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET sock_t;
#define BAD_SOCK INVALID_SOCKET
#define sock_close closesocket
#else
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
typedef int sock_t;
#define BAD_SOCK (-1)
#define sock_close close
#endif

#include "host.h"
#include "host_mem.h"
#include "plat.h"
#include "nitro/wm.h"

int pxi7_send_pointer(u32 tag, void *p);

#define MS(n) ((u64)(n) * 33514u)  // virtual cycles (the ARM9 bus clock, 33.514 MHz)
#define FRAME_CYCLES (2130u * 263u)
#define LOST_NS 10000000000ull     // a peer silent this long (real time) is gone

enum {  // WMStatus.state
    ST_READY, ST_STOP, ST_IDLE, ST_CLASS1, ST_TESTMODE, ST_SCAN, ST_CONNECT, ST_PARENT, ST_CHILD, ST_MP_PARENT,
    ST_MP_CHILD, ST_DCF_CHILD
};
enum {  // callback state codes
    SC_PARENT_START = 0, SC_BEACON_SENT = 2, SC_PARENT_NOT_FOUND = 4, SC_PARENT_FOUND = 5, SC_CONNECTED = 7,
    SC_DISCONNECTED = 9, SC_MP_START = 10, SC_MPEND_IND = 11, SC_MP_IND = 12, SC_MPACK_IND = 13, SC_PORT_SEND = 20,
    SC_PORT_RECV = 21
};
enum {  // request ids (WMMsg.id)
    API_INITIALIZE = 0, API_RESET, API_END, API_ENABLE, API_DISABLE, API_POWER_ON, API_POWER_OFF, API_SET_P_PARAM,
    API_START_PARENT, API_END_PARENT, API_START_SCAN, API_END_SCAN, API_START_CONNECT, API_DISCONNECT, API_START_MP,
    API_SET_MP_DATA, API_END_MP, API_START_DCF, API_SET_DCF_DATA, API_END_DCF, API_SET_WEPKEY, API_START_KS,
    API_END_KS, API_GET_KEYSET, API_SET_GAMEINFO, API_SET_BEACON_IND, API_MEASURE_CHANNEL = 30, API_SET_ENTRY = 33,
    API_START_SCAN_EX = 38, API_SET_WEPKEY_EX = 39, API_PORT_SEND = 0x81, API_PORT_RECV = 0x82
};
#define ERR_FAILED 1

static int sLog;
static u32 sStat[6];  // MP frames sent / received, answers sent / received, port data delivered, (spare): ACWW_WM_LOG
enum { ST_FRAMES_OUT, ST_FRAMES_IN, ST_ANSWERS_OUT, ST_ANSWERS_IN, ST_DATA_IN };
#define LOG(...) do { if (sLog) fprintf(stderr, "wm7: " __VA_ARGS__); } while (0)
#define LOG2(...) do { if (sLog > 1) fprintf(stderr, "wm7: " __VA_ARGS__); } while (0)  // (ACWW_WM_LOG=2: every packet)

// ---- messages to the ARM9: posted in order, each when its time is due
static struct { u64 due; WMMsg *m; } sOut[128];
static int sOutHead, sOutCount;
static u64 sOutLast;
static void post(WMMsg *m, u64 delay) {
    u64 due = host_cycles() + delay;
    if (due < sOutLast) due = sOutLast;
    if (sOutCount == 128) { fprintf(stderr, "wm7: too many messages for the ARM9 (%u dropped)\n", m->id); return; }
    sOutLast = due;
    sOut[(sOutHead + sOutCount) % 128].due = due;
    sOut[(sOutHead + sOutCount++) % 128].m = m;
}

// the ARM7's own message buffers (events), free again when the ARM9 has handled them (id bit 15), each with room for
// received data
#define POOL 64
static _Alignas(32) u8 sPool[POOL][0x100];
static u8 sPoolUsed[POOL];
static u8 sPoolData[POOL][0x220];
static WMMsg *event(u16 id, u8 **data) {
    for (int i = 0; i < POOL; i++) {
        WMMsg *m = (WMMsg *)sPool[i];
        if (sPoolUsed[i] && !(m->id & 0x8000)) continue;
        sPoolUsed[i] = 1;
        memset(m, 0, 0x100);
        m->id = id;
        if (data) *data = sPoolData[i];
        return m;
    }
    static int warned;
    if (!warned++) fprintf(stderr, "wm7: no free message buffer (event %u dropped)\n", id);
    return 0;
}

// ---- state
static WMStatus *sSt;  // the ARM9's status block (WM_Initialize / WM_Enable)
static u16 sState;
static u8 sMac[6];
static void set_state(u16 s) {
    sState = s;
    if (sSt) sSt->state = s;
}

// parent
static WMParentParam sParam;
static u8 sGameInfo[0x70];
static u16 sGameInfoLength;
static u32 sGgid;
static u16 sTgid;
static u8 sAttr;      // the beacon's attribute: bit 0 children may join
static u64 sNextBeacon;
static struct Child {
    int on;
    u8 mac[6];
    struct sockaddr_in addr;
    u64 seen;
    struct { u16 port, length; u8 data[0x200]; } reply[4];  // received, for the next MP sequences
    int replyHead, replyCount;
} sChild[16];  // by aid (1-15)

// child
static struct Parent {  // a parent heard (its beacons)
    int on;
    u8 mac[6];
    struct sockaddr_in addr;
    u64 seen;
    u32 ggid;
    u16 tgid, channel, beaconPeriod, parentMaxSize, childMaxSize;
    u8 attr, gameInfoLength, gameInfo[0x70];
} sParents[16];
static int sScanNext;
static struct Parent sJoined;  // the parent connected to
static u16 sMyAid;
static WMMsg *sConnectMsg;     // the StartConnect request until it's answered
static u64 sConnectRetry, sConnectGiveUp;
static struct MpFrame { u32 seq; u16 port, dest, length; u8 data[0x200]; } sMpIn[8];  // the parent's frames received
static int sMpInHead, sMpInCount;
static u64 sNextAlive;

// scan / measurement in progress
static WMMsg *sScanMsg;
static int sScanEx;
static u64 sScanDue;
static u32 sScanBuf;
static u16 sScanBufSize;
static u64 sNextProbe;

// MP
static int sMp;
static u64 sNextMp, sMpPeriod;
static u32 sSeq;
static struct Send { WMMsg *m; void *data; u16 size, dest, port, prio; void *cb, *arg; } sSend[16];
static int sSendCount;

// ---- network
static sock_t sSock = BAD_SOCK;
static u16 sPort;
#define PORT_BASE 31470
#define PORTS 8
static struct sockaddr_in sPeers[8];
static int sLan = 1;  // ACWW_NET_LAN=0: no beacons to the machine and the LAN (only the peers and the probers)
static int sPeerCount;
static struct { struct sockaddr_in addr; u64 until; } sProbers[8];

enum { PK_BEACON = 1, PK_PROBE, PK_CONNECT, PK_ACCEPT, PK_REJECT, PK_DISCONNECT, PK_MP, PK_REPLY, PK_ALIVE };
#define HDR 18  // "ACWL", version, type, source MAC, destination MAC

static void put16(u8 *p, u32 v) { p[0] = (u8)v, p[1] = (u8)(v >> 8); }
static void put32(u8 *p, u32 v) { put16(p, v), put16(p + 2, v >> 16); }
static u16 get16(const u8 *p) { return (u16)(p[0] | p[1] << 8); }
static u32 get32(const u8 *p) { return get16(p) | (u32)get16(p + 2) << 16; }

static void net_open(void) {
    if (sSock != BAD_SOCK) return;
#ifdef _WIN32
    static int started;
    if (!started) { WSADATA d; if (WSAStartup(MAKEWORD(2, 2), &d)) return; started = 1; }
#endif
    sSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sSock == BAD_SOCK) { fprintf(stderr, "wm7: no UDP socket: no wireless\n"); return; }
    int one = 1;
    setsockopt(sSock, SOL_SOCKET, SO_BROADCAST, (const char *)&one, sizeof one);
#ifdef _WIN32
    u_long nb = 1;
    ioctlsocket(sSock, FIONBIO, &nb);
#else
    fcntl(sSock, F_SETFL, fcntl(sSock, F_GETFL) | O_NONBLOCK);
    fcntl(sSock, F_SETFD, FD_CLOEXEC);  // (not kept by the program started again: host_reset_system)
#endif
    const char *e = getenv("ACWW_NET_PORT");
    int first = e ? atoi(e) : PORT_BASE, last = e ? first : PORT_BASE + PORTS - 1;
    for (int port = first; port <= last; port++) {
        struct sockaddr_in a = {0};
        a.sin_family = AF_INET;
        a.sin_addr.s_addr = htonl(INADDR_ANY);
        a.sin_port = htons((u16)port);
        if (bind(sSock, (struct sockaddr *)&a, sizeof a) == 0) { sPort = (u16)port; break; }
    }
    if (!sPort) {
        fprintf(stderr, "wm7: no free UDP port in %d-%d: no wireless\n", first, last);
        sock_close(sSock);
        sSock = BAD_SOCK;
        return;
    }
    sLan = !getenv("ACWW_NET_LAN") || atoi(getenv("ACWW_NET_LAN")) != 0;
    sPeerCount = 0;
    for (const char *p = getenv("ACWW_NET_PEER"); p && *p && sPeerCount < 8;) {
        char host[256], *colon;
        size_t n = strcspn(p, ",");
        snprintf(host, sizeof host, "%.*s", (int)n, p);
        p += n + (p[n] == ',');
        int port = PORT_BASE;
        if ((colon = strrchr(host, ':'))) *colon = 0, port = atoi(colon + 1);
        struct addrinfo hints = {0}, *res = 0;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        if (getaddrinfo(host, 0, &hints, &res) == 0 && res) {
            sPeers[sPeerCount] = *(struct sockaddr_in *)res->ai_addr;
            sPeers[sPeerCount++].sin_port = htons((u16)port);
            freeaddrinfo(res);
        } else {
            fprintf(stderr, "wm7: ACWW_NET_PEER: can't resolve %s\n", host);
        }
    }
    fprintf(stderr, "wm7: wireless on UDP port %u (MAC %02x:%02x:%02x:%02x:%02x:%02x)\n", sPort, sMac[0], sMac[1],
            sMac[2], sMac[3], sMac[4], sMac[5]);
}

static void net_close(void) {
    if (sSock == BAD_SOCK) return;
    sock_close(sSock);
    sSock = BAD_SOCK;
    sPort = 0;
    LOG("wireless off\n");
}

static u8 sPk[HDR + 32 + 0x220];
static u8 *packet(u32 type, const u8 *to) {
    memcpy(sPk, "ACWL", 4);
    sPk[4] = 1;
    sPk[5] = (u8)type;
    memcpy(sPk + 6, sMac, 6);
    if (to) memcpy(sPk + 12, to, 6);
    else memset(sPk + 12, 0xff, 6);
    return sPk + HDR;
}
static void send_to(const struct sockaddr_in *a, u32 length) {
    if (sSock == BAD_SOCK) return;
    sendto(sSock, (const char *)sPk, (int)(HDR + length), 0, (const struct sockaddr *)a, sizeof *a);
}
static void send_everywhere(u32 length) {  // every copy on the machine and the LAN, the peers and the probers
    for (int i = 0; sLan && i < PORTS; i++) {
        struct sockaddr_in a = {0};
        a.sin_family = AF_INET;
        a.sin_port = htons((u16)(PORT_BASE + i));
        a.sin_addr.s_addr = htonl(INADDR_BROADCAST);
        send_to(&a, length);
        a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        send_to(&a, length);
    }
    for (int i = 0; i < sPeerCount; i++) send_to(&sPeers[i], length);
    u64 now = host_plat_time_ns();
    for (int i = 0; i < 8; i++)
        if (sProbers[i].until > now) send_to(&sProbers[i].addr, length);
}

// ---- the parent's side
static u16 connected_mask(void) {
    u16 m = 0;
    for (int aid = 1; aid < 16; aid++)
        if (sChild[aid].on) m |= (u16)(1 << aid);
    return m;
}
static void update_children(void) {
    if (sSt) sSt->f17e = connected_mask();
}

static u32 beacon_body(u8 *p) {
    put32(p, sGgid);
    put16(p + 4, sTgid);
    put16(p + 6, sParam.channel);
    put16(p + 8, sParam.beaconPeriod);
    p[10] = sAttr;
    p[11] = sGameInfoLength;
    put16(p + 12, sParam.parentMaxSize);
    put16(p + 14, sParam.childMaxSize);
    memcpy(p + 16, sGameInfo, sGameInfoLength);
    return 16u + sGameInfoLength;
}
static void send_beacon(const struct sockaddr_in *to) {
    u8 *p = packet(PK_BEACON, 0);
    u32 n = beacon_body(p);
    if (to) send_to(to, n);
    else send_everywhere(n);
}

static void parent_event(u16 state, const u8 *mac, u16 aid) {
    WMMsg *m = event(API_START_PARENT, 0);
    if (!m) return;
    WMStartParentCallback *c = (WMStartParentCallback *)m;
    c->state = state;
    if (mac) memcpy(c->macAddress, mac, 6);
    c->aid = aid;
    post(m, 0);
}

// a child gone: notify (the StartParent callback's DISCONNECTED: not for the parent's own Disconnect / Reset), tell
// (a datagram to the child: the parent ended it), why (for the log)
static void drop_child(int aid, int notify, int tell, const char *why) {
    struct Child *c = &sChild[aid];
    if (!c->on) return;
    LOG("child %d: %s\n", aid, why);
    if (tell) {
        u8 *p = packet(PK_DISCONNECT, c->mac);
        put16(p, (u16)aid);
        send_to(&c->addr, 2);
    }
    c->on = 0;
    update_children();
    if (notify) parent_event(SC_DISCONNECTED, c->mac, (u16)aid);
}

// ---- requests that complete later
static void reply_cmd(WMMsg *m, u16 id, u16 err, u64 delay) {
    memset(m, 0, 0x100);
    m->id = id;
    m->errcode = err;
    post(m, delay);
}

// SetMPData requests waiting for an MP sequence; their result goes into their own buffer
static void finish_send(struct Send *s, u16 sent) {
    WMPortSendCallback *c = (WMPortSendCallback *)s->m;
    memset(c, 0, 0x100);
    c->apiid = API_PORT_SEND;
    c->state = SC_PORT_SEND;
    c->port = s->port;
    c->destBitmap = s->dest;
    c->sentBitmap = sent;
    c->data = (u16 *)s->data;
    c->size = s->size;
    c->seqNo = (u16)sSeq;
    c->callback = s->cb;
    c->arg = s->arg;
    post((WMMsg *)c, 0);
}
static int take_send(struct Send *out) {  // the first of the highest priority
    if (!sSendCount) return 0;
    int best = 0;
    for (int i = 1; i < sSendCount; i++)
        if (sSend[i].prio < sSend[best].prio) best = i;
    *out = sSend[best];
    memmove(&sSend[best], &sSend[best + 1], (size_t)(sSendCount - best - 1) * sizeof *sSend);
    sSendCount--;
    return 1;
}
static void flush_sends(void) {  // MP ended: the waiting requests complete without having been sent
    struct Send s;
    while (take_send(&s)) finish_send(&s, 0);
}

static void mp_event(u16 state) {
    WMMsg *m = event(API_START_MP, 0);
    if (!m) return;
    ((WMStartMPCallback *)m)->state = state;
    post(m, 0);
}

static void port_recv(u16 port, u16 aid, const u8 *mac, const u8 *data, u16 length) {
    u8 *buf;
    WMMsg *m = event(API_PORT_RECV, &buf);
    if (!m) return;
    WMPortRecvCallback *r = (WMPortRecvCallback *)m;
    if (length > 0x220) length = 0x220;
    memcpy(buf, data, length);
    r->state = SC_PORT_RECV;
    r->port = port;
    r->data = (u16 *)buf;
    r->length = length;
    r->aid = aid;
    memcpy(r->macAddress, mac, 6);
    r->seqNo = (u16)sSeq;
    r->myAid = sMyAid;
    r->connectedAidBitmap = sSt ? sSt->f17e : 0;
    post(m, 0);
    sStat[ST_DATA_IN]++;
    if (sLog > 1) {
        fprintf(stderr, "wm7: received: port %u, %u bytes from aid %u (frame %llu):", port, length, aid,
                (unsigned long long)host_frame());
        for (int i = 0; i < length && i < 24; i++) fprintf(stderr, " %02x", data[i]);
        fprintf(stderr, "\n");
    }
}

static void stop_mp(void) {
    sMp = 0;
    sMpInCount = 0;
    if (sSt) sSt->f0c = 0;
    flush_sends();
}

// an MP sequence of the parent: the children's answers to the last one, the next frame out, MPEND_IND
static void parent_mp(void) {
    sSeq++;
    for (int aid = 1; aid < 16; aid++) {
        struct Child *c = &sChild[aid];
        if (!c->on || !c->replyCount) continue;
        port_recv(c->reply[c->replyHead].port, (u16)aid, c->mac, c->reply[c->replyHead].data,
                  c->reply[c->replyHead].length);
        c->replyHead = (c->replyHead + 1) % 4;
        c->replyCount--;
    }
    struct Send s;
    int have = take_send(&s);
    u16 children = connected_mask();
    for (int aid = 1; aid < 16; aid++) {
        if (!(children & 1 << aid)) continue;
        int with = have && (s.dest & 1 << aid);
        u8 *p = packet(PK_MP, sChild[aid].mac);
        put32(p, sSeq);
        put16(p + 4, with ? s.port : 0xffff);
        put16(p + 6, have ? s.dest : 0);
        put16(p + 8, with ? s.size : 0);
        if (with) memcpy(p + 10, s.data, s.size);
        send_to(&sChild[aid].addr, 10u + (with ? s.size : 0));
        sStat[ST_FRAMES_OUT]++;
    }
    if (have) finish_send(&s, s.dest & children);
    mp_event(SC_MPEND_IND);
}

// ---- the child's side
static void connect_event(u16 state, u16 aid) {
    WMMsg *m = event(API_START_CONNECT, 0);
    if (!m) return;
    WMStartConnectCallback *c = (WMStartConnectCallback *)m;
    c->state = state;
    c->aid = aid;
    memcpy(c->macAddress, sJoined.mac, 6);
    post(m, 0);
}

static void child_lost(int tell) {
    LOG("parent %s\n", tell ? "ended the connection" : "lost (nothing heard for 10 s)");
    stop_mp();
    sMyAid = 0;
    if (sSt) sSt->f184 = 0, sSt->f17e = 0;
    set_state(ST_IDLE);
    connect_event(SC_DISCONNECTED, 0);
}

// a frame of the parent's: its data, MP_IND, the answer (the data set before), MPACK_IND
static void child_mp(struct MpFrame *f) {
    sSeq = f->seq;
    if (f->length && (f->dest & 1 << sMyAid)) port_recv(f->port, 0, sJoined.mac, f->data, f->length);
    mp_event(SC_MP_IND);
    struct Send s;
    int have = take_send(&s);
    u8 *p = packet(PK_REPLY, sJoined.mac);
    put32(p, f->seq);
    put16(p + 4, sMyAid);
    put16(p + 6, have ? s.port : 0xffff);
    put16(p + 8, have ? s.size : 0);
    if (have) memcpy(p + 10, s.data, s.size);
    send_to(&sJoined.addr, 10u + (have ? s.size : 0));
    sStat[ST_ANSWERS_OUT]++;
    if (have) finish_send(&s, 1);
    mp_event(SC_MPACK_IND);
}

// a parent's description as a scan finds it (WMBssDesc, 0xc0 bytes)
static void bss_desc(WMBssDesc *b, const struct Parent *p) {
    memset(b, 0, sizeof *b);
    u16 gi = (u16)(0x10 + p->gameInfoLength);
    b->length = (u16)((0x40 + gi + 1) / 2);
    b->rssi = 0x3c;
    memcpy(b->bssid, p->mac, 6);
    b->ssidLength = 32;
    put32(b->ssid, p->ggid);
    put16(b->ssid + 4, p->tgid);
    b->capaInfo = 0x21;
    b->rateSet.basic = 3;
    b->rateSet.support = 3;
    b->beaconPeriod = p->beaconPeriod;
    b->dtimPeriod = 1;
    b->channel = p->channel;
    b->gameInfoLength = gi;
    b->gameInfo.magicNumber = 1;
    b->gameInfo.ver = 1;
    b->gameInfo.ggid = p->ggid;
    b->gameInfo.tgid = p->tgid;
    b->gameInfo.userGameInfoLength = p->gameInfoLength;
    b->gameInfo.attribute = p->attr;
    b->gameInfo.parentMaxSize = p->parentMaxSize;
    b->gameInfo.childMaxSize = p->childMaxSize;
    memcpy(b->gameInfo.userGameInfo, p->gameInfo, p->gameInfoLength);
}

static void finish_scan(void) {
    WMMsg *m = sScanMsg;
    sScanMsg = 0;
    u64 now = host_plat_time_ns();
    for (int i = 0; i < 16; i++)
        if (sParents[i].on && now - sParents[i].seen > 3000000000ull) sParents[i].on = 0;
    if (!sScanEx) {
        WMBssDesc *buf = HostPtr32_Get(sScanBuf);
        struct Parent *found = 0;
        for (int i = 0; i < 16 && !found; i++) {  // one parent per scan, in turn
            struct Parent *p = &sParents[(sScanNext + i) % 16];
            if (p->on) found = p, sScanNext = (sScanNext + i + 1) % 16;
        }
        memset(m, 0, 0x100);
        m->id = API_START_SCAN;
        WMStartScanCallback *c = (WMStartScanCallback *)m;
        c->state = found ? SC_PARENT_FOUND : SC_PARENT_NOT_FOUND;
        if (found) {
            bss_desc(buf, found);
            memcpy(c->macAddress, found->mac, 6);
            c->channel = found->channel;
            c->linkLevel = 3;
        }
        post(m, 0);
        return;
    }
    u8 *buf = HostPtr32_Get(sScanBuf);
    memset(m, 0, 0x100);
    m->id = API_START_SCAN_EX;
    WMStartScanExCallback *c = (WMStartScanExCallback *)m;
    u32 used = 0;
    for (int i = 0; i < 16; i++) {
        if (!sParents[i].on || used + sizeof(WMBssDesc) > sScanBufSize) continue;
        WMBssDesc *b = (WMBssDesc *)(buf + used);
        bss_desc(b, &sParents[i]);
        c->bssDesc[c->bssDescCount] = b;
        c->linkLevel[c->bssDescCount++] = 3;
        used += sizeof(WMBssDesc);
    }
    c->state = c->bssDescCount ? SC_PARENT_FOUND : SC_PARENT_NOT_FOUND;
    post(m, 0);
}

static void finish_connect(int aid, u16 parentMax, u16 childMax) {
    WMMsg *m = sConnectMsg;
    sConnectMsg = 0;
    if (aid <= 0) {
        LOG("connecting failed\n");
        set_state(ST_IDLE);
        reply_cmd(m, API_START_CONNECT, ERR_FAILED, 0);
        return;
    }
    LOG("connected as aid %d\n", aid);
    sMyAid = (u16)aid;
    sJoined.parentMaxSize = parentMax;
    sJoined.childMaxSize = childMax;
    sJoined.seen = host_plat_time_ns();
    if (sSt) {
        sSt->f184 = (u16)aid;
        sSt->fc2 = 1;
        sSt->f190 = parentMax;
        sSt->f18e = (u16)(childMax + 2);
        sSt->f17e = 1;
        sSt->fb8 = 3;
    }
    set_state(ST_CHILD);
    memset(m, 0, 0x100);
    m->id = API_START_CONNECT;
    WMStartConnectCallback *c = (WMStartConnectCallback *)m;
    c->state = SC_CONNECTED;
    c->aid = (u16)aid;
    memcpy(c->macAddress, sJoined.mac, 6);
    c->parentSize = parentMax;
    c->childSize = childMax;
    post(m, 0);
    sNextAlive = host_cycles();
}

// leaving whatever the radio does (Reset, End, EndParent)
static void leave_all(void) {
    if (sScanMsg) finish_scan();
    if (sConnectMsg) finish_connect(0, 0, 0);
    stop_mp();
    for (int aid = 1; aid < 16; aid++) drop_child(aid, 0, 1, "disconnected");
    if (sState == ST_CHILD || sState == ST_MP_CHILD) {
        u8 *p = packet(PK_DISCONNECT, sJoined.mac);
        put16(p, sMyAid);
        send_to(&sJoined.addr, 2);
    }
    sMyAid = 0;
    if (sSt) sSt->f184 = 0, sSt->f17e = 0;
}

// ---- datagrams
static int same_mac(const u8 *a, const u8 *b) { return !memcmp(a, b, 6); }

static void receive(const u8 *pk, int n, const struct sockaddr_in *from) {
    if (n < HDR || memcmp(pk, "ACWL", 4) || pk[4] != 1) return;
    const u8 *src = pk + 6, *dst = pk + 12, *p = pk + HDR;
    int len = n - HDR;
    static const u8 kAll[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    if (same_mac(src, sMac) || (!same_mac(dst, sMac) && !same_mac(dst, kAll))) return;
    u64 now = host_plat_time_ns();
    int parent = sState == ST_PARENT || sState == ST_MP_PARENT;
    int child = sState == ST_CHILD || sState == ST_MP_CHILD;
    int aid = 0;
    if (parent)
        for (int i = 1; i < 16; i++)
            if (sChild[i].on && same_mac(sChild[i].mac, src)) aid = i;
    switch (pk[5]) {
    case PK_BEACON: {
        if (len < 16 || p[11] > 0x70 || len < 16 + p[11]) return;
        struct Parent *e = 0;
        for (int i = 0; i < 16 && !e; i++)
            if (sParents[i].on && same_mac(sParents[i].mac, src)) e = &sParents[i];
        for (int i = 0; i < 16 && !e; i++)
            if (!sParents[i].on) e = &sParents[i];
        if (!e) return;
        if (!e->on) LOG("a parent: %02x:%02x:%02x:%02x:%02x:%02x\n", src[0], src[1], src[2], src[3], src[4], src[5]);
        e->on = 1;
        memcpy(e->mac, src, 6);
        e->addr = *from;
        e->seen = now;
        e->ggid = get32(p);
        e->tgid = get16(p + 4);
        e->channel = get16(p + 6);
        e->beaconPeriod = get16(p + 8);
        e->attr = p[10];
        e->gameInfoLength = p[11];
        e->parentMaxSize = get16(p + 12);
        e->childMaxSize = get16(p + 14);
        memcpy(e->gameInfo, p + 16, p[11]);
        if (child && same_mac(src, sJoined.mac)) sJoined.seen = now;
        break;
    }
    case PK_PROBE: {
        int slot = 0;
        for (int i = 0; i < 8; i++) {
            if (sProbers[i].addr.sin_addr.s_addr == from->sin_addr.s_addr &&
                sProbers[i].addr.sin_port == from->sin_port) {
                slot = i;
                break;
            }
            if (sProbers[i].until < sProbers[slot].until) slot = i;
        }
        sProbers[slot].addr = *from;
        sProbers[slot].until = now + 5000000000ull;
        if (parent) send_beacon(from);
        break;
    }
    case PK_CONNECT:
        if (!parent) return;
        if (!aid && (sAttr & 1)) {
            for (int i = 1; i <= sParam.maxEntry && i < 16 && !aid; i++)
                if (!sChild[i].on) aid = i;
            if (aid) {
                struct Child *c = &sChild[aid];
                memset(c, 0, sizeof *c);
                c->on = 1;
                memcpy(c->mac, src, 6);
                c->addr = *from;
                c->seen = now;
                update_children();
                LOG("child %d joined: %02x:%02x:%02x:%02x:%02x:%02x\n", aid, src[0], src[1], src[2], src[3], src[4],
                    src[5]);
                parent_event(SC_CONNECTED, src, (u16)aid);
            }
        }
        if (aid) {
            u8 *q = packet(PK_ACCEPT, src);
            put16(q, (u16)aid);
            put16(q + 2, sParam.parentMaxSize);
            put16(q + 4, sParam.childMaxSize);
            send_to(from, 6);
        } else {
            packet(PK_REJECT, src);
            send_to(from, 0);
        }
        break;
    case PK_ACCEPT:
        if (sConnectMsg && len >= 6 && same_mac(src, sJoined.mac)) {
            sJoined.addr = *from;
            finish_connect(get16(p), get16(p + 2), get16(p + 4));
        }
        break;
    case PK_REJECT:
        if (sConnectMsg && same_mac(src, sJoined.mac)) finish_connect(0, 0, 0);
        break;
    case PK_DISCONNECT:
        if (parent && aid) drop_child(aid, 1, 0, "left");
        else if (child && same_mac(src, sJoined.mac)) child_lost(1);
        break;
    case PK_MP:
        if (!child || !same_mac(src, sJoined.mac) || len < 10) return;
        sJoined.seen = now;
        if (sState != ST_MP_CHILD || sMpInCount == 8) return;
        {
            struct MpFrame *f = &sMpIn[(sMpInHead + sMpInCount) % 8];
            f->seq = get32(p);
            f->port = get16(p + 4);
            f->dest = get16(p + 6);
            f->length = get16(p + 8);
            if (f->length > 0x200 || len < 10 + f->length) return;
            memcpy(f->data, p + 10, f->length);
            sMpInCount++;
            sStat[ST_FRAMES_IN]++;
        }
        break;
    case PK_REPLY:
        if (!aid || len < 10) return;
        sStat[ST_ANSWERS_IN]++;
        sChild[aid].seen = now;
        sChild[aid].addr = *from;
        {
            u16 port = get16(p + 6), length = get16(p + 8);
            struct Child *c = &sChild[aid];
            if (port == 0xffff || length == 0 || length > 0x200 || len < 10 + length || c->replyCount == 4 || !sMp)
                return;
            int i = (c->replyHead + c->replyCount++) % 4;
            c->reply[i].port = port;
            c->reply[i].length = length;
            memcpy(c->reply[i].data, p + 10, length);
        }
        break;
    case PK_ALIVE:
        if (aid) sChild[aid].seen = now;
        else if (child && same_mac(src, sJoined.mac)) sJoined.seen = now;
        break;
    }
}

static void net_poll(void) {
    if (sSock == BAD_SOCK) return;
    for (int i = 0; i < 64; i++) {
        u8 buf[2048];
        struct sockaddr_in from;
        socklen_t fl = sizeof from;
        int n = (int)recvfrom(sSock, (char *)buf, sizeof buf, 0, (struct sockaddr *)&from, &fl);
        if (n < 0) break;
        receive(buf, n, &from);
    }
}

// ---- requests
static u32 P(WMMsg *m, int i) { return ((u32 *)m)[1 + i]; }

void wm7_receive(void *request) {
    WMMsg *m = request;
    if (!m) return;
    u16 id = m->id & 0x7fff;
    const u64 soon = MS(1) / 2;
    if (id != API_SET_MP_DATA) LOG("request %u (state %u, frame %llu)\n", id, sState, (unsigned long long)host_frame());
    switch (id) {
    case API_INITIALIZE:
    case API_ENABLE:
        sSt = HostPtr32_Get(P(m, 1));
        net_open();
        set_state(id == API_INITIALIZE ? ST_IDLE : ST_STOP);
        reply_cmd(m, id, 0, soon);
        break;
    case API_DISABLE:
    case API_END:
        leave_all();
        net_close();
        set_state(ST_READY);
        reply_cmd(m, id, 0, soon);
        break;
    case API_POWER_ON: set_state(ST_IDLE); reply_cmd(m, id, 0, MS(2)); break;
    case API_POWER_OFF: leave_all(); set_state(ST_STOP); reply_cmd(m, id, 0, soon); break;
    case API_RESET:
        leave_all();
        set_state(ST_IDLE);
        reply_cmd(m, id, 0, soon);
        break;
    case API_SET_P_PARAM: {
        const WMParentParam *p = HostPtr32_Get(P(m, 0));
        sParam = *p;
        sGameInfoLength = (u16)(p->userGameInfoLength > 0x70 ? 0x70 : p->userGameInfoLength);
        if (sGameInfoLength) memcpy(sGameInfo, p->userGameInfo, sGameInfoLength);
        sGgid = p->ggid;
        sTgid = p->tgid;
        sAttr = (u8)((p->entryFlag ? 1 : 0) | (p->multiBootFlag ? 2 : 0) | (p->KS_Flag ? 4 : 0) | (p->CS_Flag ? 8 : 0));
        reply_cmd(m, id, 0, soon);
        break;
    }
    case API_START_PARENT:
        memset(sChild, 0, sizeof sChild);
        sMyAid = 0;
        if (sSt) {
            sSt->f184 = 0;
            sSt->f17e = 0;
            sSt->ff4 = sParam.maxEntry;
            sSt->f190 = sParam.childMaxSize;
            sSt->f18e = (u16)(sParam.parentMaxSize + 4);
            sSt->fb8 = 3;
        }
        set_state(ST_PARENT);
        LOG("parent: ggid %08x tgid %04x, %u children at most\n", sGgid, sTgid, sParam.maxEntry);
        memset(m, 0, 0x100);
        m->id = id;
        ((WMStartParentCallback *)m)->state = SC_PARENT_START;
        post(m, soon);
        sNextBeacon = host_cycles() + MS(sParam.beaconPeriod ? sParam.beaconPeriod : 200);
        send_beacon(0);
        break;
    case API_END_PARENT:
        leave_all();
        set_state(ST_IDLE);
        reply_cmd(m, id, 0, soon);
        break;
    case API_START_SCAN:
    case API_START_SCAN_EX: {
        u16 time;
        if (id == API_START_SCAN) {
            WMScanReq *r = (WMScanReq *)m;
            sScanBuf = r->scanBuf, sScanBufSize = sizeof(WMBssDesc), time = r->maxChannelTime;
        } else {
            WMScanExReq *r = (WMScanExReq *)m;
            sScanBuf = r->scanBuf, sScanBufSize = r->scanBufSize, time = r->maxChannelTime;
        }
        sScanMsg = m;
        sScanEx = id == API_START_SCAN_EX;
        sScanDue = host_cycles() + MS(time < 10 ? 10 : time);
        set_state(ST_SCAN);
        if (sNextProbe < host_cycles()) sNextProbe = host_cycles();
        break;
    }
    case API_END_SCAN:
        if (sScanMsg) finish_scan();
        set_state(ST_IDLE);
        reply_cmd(m, id, 0, soon);
        break;
    case API_START_CONNECT: {
        const WMBssDesc *b = HostPtr32_Get(((WMConnectReq *)m)->pInfo);
        struct Parent *e = 0;
        for (int i = 0; i < 16 && !e; i++)
            if (sParents[i].on && same_mac(sParents[i].mac, b->bssid)) e = &sParents[i];
        set_state(ST_CONNECT);
        sConnectMsg = m;
        if (!e) { finish_connect(0, 0, 0); break; }
        sJoined = *e;
        sConnectRetry = host_cycles();
        sConnectGiveUp = host_cycles() + MS(3000);
        LOG("connecting to %02x:%02x:%02x:%02x:%02x:%02x\n", e->mac[0], e->mac[1], e->mac[2], e->mac[3], e->mac[4],
            e->mac[5]);
        break;
    }
    case API_DISCONNECT: {
        u32 mask = P(m, 0);
        if (sState == ST_PARENT || sState == ST_MP_PARENT) {
            for (int aid = 1; aid < 16; aid++)
                if (mask & 1u << aid) drop_child(aid, 0, 1, "disconnected");
        } else {
            leave_all();
            set_state(ST_IDLE);
        }
        reply_cmd(m, id, 0, soon);
        break;
    }
    case API_START_MP: {
        u32 freq = P(m, 4);
        sMp = 1;
        sSeq = 0;
        sMpInCount = 0;
        sMpPeriod = FRAME_CYCLES / (freq ? (freq > 16 ? 16 : freq) : 4);
        sNextMp = host_cycles() + sMpPeriod;
        if (sSt) sSt->f0c = 1;
        set_state(sState == ST_PARENT ? ST_MP_PARENT : ST_MP_CHILD);
        LOG("MP started (%u per frame)\n", freq);
        memset(m, 0, 0x100);
        m->id = id;
        ((WMStartMPCallback *)m)->state = SC_MP_START;
        post(m, soon);
        break;
    }
    case API_SET_MP_DATA: {
        if (sSendCount == 16 || !sMp) {  // (the ARM9 checks the state; this is only a guard)
            struct Send s = {m, HostPtr32_Get(P(m, 0)), (u16)P(m, 1), (u16)P(m, 2), (u16)P(m, 3), (u16)P(m, 4),
                             HostPtr32_Get(P(m, 5)), HostPtr32_Get(P(m, 6))};
            finish_send(&s, 0);
            break;
        }
        LOG2("send: port %u, %u bytes to %04x (frame %llu)\n", P(m, 3), P(m, 1), P(m, 2),
             (unsigned long long)host_frame());
        struct Send *s = &sSend[sSendCount++];
        s->m = m;
        s->data = HostPtr32_Get(P(m, 0));
        s->size = (u16)P(m, 1);
        s->dest = (u16)P(m, 2);
        s->port = (u16)P(m, 3);
        s->prio = (u16)P(m, 4);
        s->cb = HostPtr32_Get(P(m, 5));
        s->arg = HostPtr32_Get(P(m, 6));
        if (s->size > 0x200) s->size = 0x200;
        break;
    }
    case API_END_MP:
        stop_mp();
        set_state(sState == ST_MP_PARENT ? ST_PARENT : sState == ST_MP_CHILD ? ST_CHILD : sState);
        reply_cmd(m, id, 0, soon);
        break;
    case API_SET_GAMEINFO: {
        u16 n = (u16)P(m, 1);
        if (n > 0x70) n = 0x70;
        memcpy(sGameInfo, HostPtr32_Get(P(m, 0)), n);
        sGameInfoLength = n;
        sGgid = P(m, 2);
        sTgid = (u16)P(m, 3);
        sAttr = (u8)P(m, 4);
        reply_cmd(m, id, 0, soon);
        break;
    }
    case API_SET_ENTRY:
        sAttr = (u8)((sAttr & ~1) | (P(m, 0) ? 1 : 0));
        reply_cmd(m, id, 0, soon);
        break;
    case API_MEASURE_CHANNEL: {  // every channel is quiet
        u16 channel = ((u16 *)m)[3], time = ((u16 *)m)[4];
        memset(m, 0, 0x100);
        m->id = id;
        ((WMMeasureChannelCallback *)m)->channel = channel;
        ((WMMeasureChannelCallback *)m)->ccaBusyRatio = 0;
        post(m, MS(time < 1 ? 1 : time));
        break;
    }
    case API_START_DCF:
    case API_SET_DCF_DATA:
    case API_END_DCF:  // (an access point: never found)
        reply_cmd(m, id, ERR_FAILED, soon);
        break;
    default:  // WEP keys, lifetime, beacon indications, ...: accepted
        reply_cmd(m, id, 0, soon);
        break;
    }
}

// ---- time
static u64 sNextPoll;
u64 wm7_next_due(void) {
    u64 d = ~0ull;
    if (sOutCount) d = sOut[sOutHead].due;
    if (sSock == BAD_SOCK) return d;
    if (sNextPoll < d) d = sNextPoll;
    if (sScanMsg && sScanDue < d) d = sScanDue;
    if (sMp && (sState == ST_MP_PARENT) && sNextMp < d) d = sNextMp;
    if ((sState == ST_PARENT || sState == ST_MP_PARENT) && sNextBeacon < d) d = sNextBeacon;
    return d;
}

void wm7_run(u64 now) {
    HOST_PROF(PROF_ARM7);
    static u64 sNextStat;
    if (sLog && sMp && now >= sNextStat) {
        sNextStat = now + FRAME_CYCLES * 60;
        LOG("MP: frames out %u in %u, answers out %u in %u, data delivered %u (frame %llu)\n", sStat[0], sStat[1],
            sStat[2], sStat[3], sStat[4], (unsigned long long)host_frame());
    }
    if (sSock != BAD_SOCK && now >= sNextPoll) {
        sNextPoll = now + MS(2);
        net_poll();
        u64 real = host_plat_time_ns();
        if (sState == ST_PARENT || sState == ST_MP_PARENT) {
            for (int aid = 1; aid < 16; aid++)
                if (sChild[aid].on && real - sChild[aid].seen > LOST_NS)
                    drop_child(aid, 1, 1, "lost (nothing heard for 10 s)");
        }
        if ((sState == ST_CHILD || sState == ST_MP_CHILD) && real - sJoined.seen > LOST_NS) child_lost(0);
        if (sState == ST_CHILD || sState == ST_MP_CHILD) {
            if (now >= sNextAlive) {
                sNextAlive = now + MS(1000);
                u8 *p = packet(PK_ALIVE, sJoined.mac);
                put16(p, sMyAid);
                send_to(&sJoined.addr, 2);
            }
            if (sMpInCount) {  // one of the parent's frames each time: the ARM9 handles its callbacks in between
                child_mp(&sMpIn[sMpInHead]);
                sMpInHead = (sMpInHead + 1) % 8;
                sMpInCount--;
            }
        }
        if (sConnectMsg && now >= sConnectRetry) {
            if (now >= sConnectGiveUp) finish_connect(0, 0, 0);
            else {
                sConnectRetry = now + MS(100);
                packet(PK_CONNECT, sJoined.mac);
                send_to(&sJoined.addr, 0);
            }
        }
        if (sState == ST_SCAN && sPeerCount && now >= sNextProbe) {
            sNextProbe = now + MS(500);
            packet(PK_PROBE, 0);
            for (int i = 0; i < sPeerCount; i++) send_to(&sPeers[i], 0);
        }
    }
    if (sScanMsg && now >= sScanDue) finish_scan();
    if (sState == ST_PARENT || sState == ST_MP_PARENT) {
        if (now >= sNextBeacon) {
            u64 period = MS(sParam.beaconPeriod ? sParam.beaconPeriod : 200);
            sNextBeacon = now - sNextBeacon > period ? now + period : sNextBeacon + period;
            send_beacon(0);
            parent_event(SC_BEACON_SENT, 0, 0);
            for (int aid = 1; aid < 16; aid++)  // (children before MP: they hear from the parent)
                if (sChild[aid].on && !sMp) {
                    u8 *p = packet(PK_ALIVE, sChild[aid].mac);
                    put16(p, (u16)aid);
                    send_to(&sChild[aid].addr, 2);
                }
        }
        if (sMp && sState == ST_MP_PARENT && now >= sNextMp) {
            sNextMp = now - sNextMp > sMpPeriod * 4 ? now + sMpPeriod : sNextMp + sMpPeriod;
            parent_mp();
        }
    }
    while (sOutCount && sOut[sOutHead].due <= now) {
        if (!pxi7_send_pointer(10, sOut[sOutHead].m)) break;  // (the FIFO is full: later)
        sOutHead = (sOutHead + 1) % 128;
        sOutCount--;
    }
}

// ---- the MAC address and the channels (the firmware's user settings as the ARM7 copies them at start-up)
static u32 fnv(u32 h, const char *s) {
    for (; s && *s; s++) h = (h ^ (u8)*s) * 16777619u;
    return h;
}
void wm7_init(const char *savePath) {
    sLog = getenv("ACWW_WM_LOG") ? atoi(getenv("ACWW_WM_LOG")) : 0;
    u8 mac[6] = {0x00, 0x09, 0xbf, 0x00, 0x00, 0x01};  // (Nintendo's prefix)
    const char *e = getenv("ACWW_MAC");
    unsigned v[6];
    if (e && sscanf(e, "%x:%x:%x:%x:%x:%x", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5]) == 6) {
        for (int i = 0; i < 6; i++) mac[i] = (u8)v[i];
    } else if (!getenv("ACWW_HEADLESS")) {
        char host[256] = "";
#ifdef _WIN32
        snprintf(host, sizeof host, "%s", getenv("COMPUTERNAME") ? getenv("COMPUTERNAME") : "");
#else
        gethostname(host, sizeof host - 1);
#endif
        u32 h = fnv(fnv(2166136261u, host), savePath);
        mac[3] = (u8)(h >> 16), mac[4] = (u8)(h >> 8), mac[5] = (u8)h;
    }
    memcpy(sMac, mac, 6);
    u8 *u = gHostSysWork.nvramUserInfo;
    memcpy(u + 0x74, mac, 6);
    u[0x7a] = 0x41, u[0x7b] = 0x10;  // the channels allowed: 1, 7 and 13 (bit n-1: channel n)
}
