/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_netxduo.c
  * @author  MCD Application Team
  * @brief   NetXDuo applicative file
  ******************************************************************************
    * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_netxduo.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "nx_stm32_eth_config.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* Define Threadx global data structures. */
TX_THREAD AppUDPThread;
TX_THREAD AppUDPListeningThread;
/* Define NetX global data structures. */
NX_UDP_SOCKET UDPSocket;
NX_UDP_SOCKET UDPListeningSocket;
ULONG IpAddress;
ULONG NetMask;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TX_THREAD      NxAppThread;
NX_PACKET_POOL NxAppPool;
NX_IP          NetXDuoEthIpInstance;
/* USER CODE BEGIN PV */
extern UCHAR memory_area[MEMORY_SIZE];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static VOID nx_app_thread_entry (ULONG thread_input);
/* USER CODE BEGIN PFP */
/* UDP thread entry */
static VOID app_UDP_thread_entry(ULONG thread_input);
static VOID app_UDP_listening_thread_entry(ULONG thread_input);

//void SystemClock_Restore(void);
/* USER CODE END PFP */

/**
  * @brief  Application NetXDuo Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT MX_NetXDuo_Init(VOID *memory_ptr)
{
  UINT ret = NX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

   /* USER CODE BEGIN App_NetXDuo_MEM_POOL */
  (void)byte_pool;
  /* USER CODE END App_NetXDuo_MEM_POOL */
  /* USER CODE BEGIN 0 */
  printf("Application started...\n");
  /* USER CODE END 0 */

  /* Initialize the NetXDuo system. */
  CHAR *pointer;
  nx_system_initialize();

    /* Allocate the memory for packet_pool.  */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_APP_PACKET_POOL_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the Packet pool to be used for packet allocation,
   * If extra NX_PACKET are to be used the NX_APP_PACKET_POOL_SIZE should be increased
   */
  ret = nx_packet_pool_create(&NxAppPool, "NetXDuo App Pool", DEFAULT_PAYLOAD_SIZE, pointer, NX_APP_PACKET_POOL_SIZE);

  if (ret != NX_SUCCESS)
  {
    return NX_POOL_ERROR;
  }

    /* Allocate the memory for Ip_Instance */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, Nx_IP_INSTANCE_THREAD_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

   /* Create the main NX_IP instance */
  ret = nx_ip_create(&NetXDuoEthIpInstance, "NetX Ip instance", NX_APP_DEFAULT_IP_ADDRESS, NX_APP_DEFAULT_NET_MASK, &NxAppPool, nx_stm32_eth_driver,
                     pointer, Nx_IP_INSTANCE_THREAD_SIZE, NX_APP_INSTANCE_PRIORITY);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

    /* Allocate the memory for ARP */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, DEFAULT_ARP_CACHE_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Enable the ARP protocol and provide the ARP cache size for the IP instance */

  /* USER CODE BEGIN ARP_Protocol_Initialization */

  /* USER CODE END ARP_Protocol_Initialization */

  ret = nx_arp_enable(&NetXDuoEthIpInstance, (VOID *)pointer, DEFAULT_ARP_CACHE_SIZE);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* Enable the ICMP */

  /* USER CODE BEGIN ICMP_Protocol_Initialization */

  /* USER CODE END ICMP_Protocol_Initialization */

  ret = nx_icmp_enable(&NetXDuoEthIpInstance);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* Enable the UDP protocol required for  DHCP communication */

  /* USER CODE BEGIN UDP_Protocol_Initialization */

  /* USER CODE END UDP_Protocol_Initialization */

  ret = nx_udp_enable(&NetXDuoEthIpInstance);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

   /* Allocate the memory for main thread   */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_APP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the main thread */
  ret = tx_thread_create(&NxAppThread, "NetXDuo App thread", nx_app_thread_entry , 0, pointer, NX_APP_THREAD_STACK_SIZE,
                         NX_APP_THREAD_PRIORITY, NX_APP_THREAD_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);

  if (ret != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  /* USER CODE BEGIN MX_NetXDuo_Init */
  /* Allocate the app UDP thread entry pool. */
  ret = tx_byte_allocate(byte_pool, (VOID **) &pointer, 2 * DEFAULT_MEMORY_SIZE, TX_NO_WAIT);

  if (ret != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  /* create the UDP server thread */
  ret = tx_thread_create(&AppUDPThread, "App UDP Thread", app_UDP_thread_entry, 0, pointer, 2 * DEFAULT_MEMORY_SIZE,
	  				     DEFAULT_PRIORITY, DEFAULT_PRIORITY, TX_NO_TIME_SLICE, TX_DONT_START);

  if (ret != TX_SUCCESS)
  {
	return TX_THREAD_ERROR;
  }

  /* Allocate the memory for Listening thread   */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer,2 *  DEFAULT_MEMORY_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
  	return TX_POOL_ERROR;
  }

  /* create the Listening thread */
  ret = tx_thread_create(&AppUDPListeningThread, "App Listening Thread", app_UDP_listening_thread_entry, 0, pointer, 2 * DEFAULT_MEMORY_SIZE,
		  	  	  	  	 DEFAULT_PRIORITY, DEFAULT_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);

  if (ret != TX_SUCCESS)
  {
  	return TX_THREAD_ERROR;
  }

  /* USER CODE END MX_NetXDuo_Init */

  return ret;
}

/**
* @brief  Main thread entry.
* @param thread_input: ULONG user argument used by the thread entry
* @retval none
*/
static VOID nx_app_thread_entry (ULONG thread_input)
{
  /* USER CODE BEGIN Nx_App_Thread_Entry 0 */
  UINT ret = NX_SUCCESS;

  /* get IP address */
  ret = nx_ip_address_get(&NetXDuoEthIpInstance, &IpAddress, &NetMask);

  /* print the IP address */
  PRINT_IP_ADDRESS(IpAddress);

  if (ret != TX_SUCCESS)
  {
    Error_Handler();
  }

  /* Now the network is correctly initialized, start the UDP server thread */
  tx_thread_resume(&AppUDPThread);

  /* this thread is not needed any more, we relinquish it */
  tx_thread_relinquish();
  /* USER CODE END Nx_App_Thread_Entry 0 */

}
/* USER CODE BEGIN 1 */

/* Send UDP packets stream for all memory content */
static VOID app_UDP_thread_entry(ULONG thread_input)
{
  UINT ret;
  NX_PACKET *data_packet;
  UINT pkt_number = 0;	/* packet number */
  UINT pkt_numeber_be;  /* conversion in network format*/
  UINT current_packet_size;
  UINT offset = 0;
  UINT packet_size = 1472;  /* Maximum safe packet size for Ethernet without fragmentation */
  UINT header_size = sizeof(UINT);  /* Size of the packet number header */

  /* create the UDP socket */
  ret = nx_udp_socket_create(&NetXDuoEthIpInstance, &UDPSocket, "UDP Client Socket", NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, QUEUE_MAX_SIZE);
  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

  /* bind UDP socket to the DEFAULT PORT */
  ret = nx_udp_socket_bind(&UDPSocket, UDP_CLIENT_PORT, TX_WAIT_FOREVER);
  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

  while (offset < MEMORY_SIZE)
  {
    /* create the packet to send over the UDP socket */
    ret = nx_packet_allocate(&NxAppPool, &data_packet, NX_UDP_PACKET, TX_WAIT_FOREVER);
    if (ret != NX_SUCCESS)
    {
      Error_Handler();
    }

    /* Determine the size of the current packet */
    current_packet_size = (MEMORY_SIZE - offset) > (packet_size - header_size) ? (packet_size - header_size) : (MEMORY_SIZE - offset);

    /* Conversion in network format */
    pkt_numeber_be = htonl(pkt_number);

    /* Append packet number to the packet */
    ret = nx_packet_data_append(data_packet, (VOID *) &pkt_numeber_be, header_size, &NxAppPool, TX_WAIT_FOREVER);
    if (ret != NX_SUCCESS)
    {
      Error_Handler();
    }

    /* Append data from the memory area to the packet */
    ret = nx_packet_data_append(data_packet, (VOID *)(memory_area + offset), current_packet_size, &NxAppPool, TX_WAIT_FOREVER);
    if (ret != NX_SUCCESS)
    {
      Error_Handler();
    }

    /* Send the packet */
    ret = nx_udp_socket_send(&UDPSocket, data_packet, UDP_SERVER_ADDRESS, UDP_SERVER_PORT);
    if (ret != NX_SUCCESS)
    {
      Error_Handler();
    }

    /* Move the offset for the next packet */
    offset += current_packet_size;
	  /* Increase sequence number of the packets */
    pkt_number++;
  }
}

/**
* @brief  Listening thread entry
* @param thread_input: ULONG thread parameter
* @retval none
*/
static VOID app_UDP_listening_thread_entry(ULONG thread_input)
{
  UINT ret;
  NX_PACKET *server_packet;

  /* create the UDP socket */
  ret = nx_udp_socket_create(&NetXDuoEthIpInstance, &UDPListeningSocket, "UDP Listening Socket", NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, QUEUE_MAX_SIZE);
  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

  /* bind UDP socket to the UDP CLIENT PORT */
//  ret = nx_udp_socket_bind(&UDPListeningSocket, UDP_CLIENT_PORT, TX_WAIT_FOREVER);
//  if (ret != NX_SUCCESS)
//  {
//    Error_Handler();
//  }

  /* wait to receive response from the server */
  ret = nx_udp_socket_receive(&UDPListeningSocket, &server_packet, NX_APP_DEFAULT_TIMEOUT);
  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

  /* unbind the socket and delete it */
  nx_udp_socket_unbind(&UDPListeningSocket);
  nx_udp_socket_delete(&UDPListeningSocket);

  /* Put micro in standby mode */
  standby();
}

/* USER CODE END 1 */
