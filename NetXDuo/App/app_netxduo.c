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
TX_THREAD AppLinkThread;
TX_THREAD AppUDPThread2;
/* Define NetX global data structures. */
NX_UDP_SOCKET UDPSocket;
NX_UDP_SOCKET UDPSocket2;
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

/* Link thread entry */
static VOID app_link_thread_entry(ULONG thread_input);

static VOID app_UDP_thread_entry_second(ULONG thread_input);

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
	 HAL_GPIO_TogglePin(LED1_GREEN_GPIO_Port, LED1_GREEN_Pin);
	  UINT ret;

	  NX_PACKET *server_packet;
	  NX_PACKET *data_packet;
	  ULONG sent = 0;
	  UINT pkt_number_new;
	  UINT pkt_number = 0;	/* packet number */
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
	  ret = nx_udp_socket_bind(&UDPSocket, 6000, TX_WAIT_FOREVER);
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
	    ULONG current_packet_size = (MEMORY_SIZE - offset) > (packet_size - header_size) ? (packet_size - header_size) : (MEMORY_SIZE - offset);


	    pkt_number_new = htonl(pkt_number);

	    /* Append packet number to the packet */
	    ret = nx_packet_data_append(data_packet, (VOID *) &pkt_number_new, header_size, &NxAppPool, TX_WAIT_FOREVER);
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

	    tx_thread_sleep(10);

	  }





	  	 /* Size of the packet number header */
	  	  nx_udp_socket_create(&NetXDuoEthIpInstance, &UDPSocket2, "UDP Client Socket", NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, QUEUE_MAX_SIZE);

	  	  /* create the UDP socket */
	  	  nx_udp_socket_create(&NetXDuoEthIpInstance, &UDPSocket2, "UDP Client Socket", NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, QUEUE_MAX_SIZE);



	  	  /* bind UDP socket to the DEFAULT PORT */
	  	  ret = nx_udp_socket_bind(&UDPSocket2, 6001, NX_WAIT_FOREVER);
	  	  ret = 0;

	  	ret = nx_udp_socket_receive(&UDPSocket2, &server_packet, 100);

	  	  /* wait to receive response from the server */

		  HAL_GPIO_TogglePin(LED1_GREEN_GPIO_Port, LED1_GREEN_Pin);

	  	  HAL_GPIO_TogglePin(LED3_RED_GPIO_Port, LED3_RED_Pin);

	  	  	/* unbind the socket and delete it */
	  	  	nx_udp_socket_unbind(&UDPSocket);
	  		nx_udp_socket_delete(&UDPSocket);
	  	  	nx_udp_socket_unbind(&UDPSocket2);
	  	  	nx_udp_socket_delete(&UDPSocket2);

	  	    HAL_PWR_DisableWakeUpPin(PWR_WKUP1);
	  	    HAL_PWR_EnableWakeUpPin(PWR_WKUP1);
	  	    HAL_PWR_EnterSTANDBYMode();
  /* USER CODE END Nx_App_Thread_Entry 0 */

}

/* USER CODE END 1 */
