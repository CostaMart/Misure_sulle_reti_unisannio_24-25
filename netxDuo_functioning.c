/*questo file contiene una versione del netxDuo che lavora su due thread. invia 35 messaggi mentre sull'altro thread ascolta sulla porta 6001 per la ricezione di un messaggio di corretta ricezione*/

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

  /* Allocate the memory for Link thread   */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer,2 *  DEFAULT_MEMORY_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
	return TX_POOL_ERROR;
  }

  /* create the Link thread */
  ret = tx_thread_create(&AppLinkThread, "App Link Thread", app_link_thread_entry, 0, pointer, 2 * DEFAULT_MEMORY_SIZE,
					     LINK_PRIORITY, LINK_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);




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
//static VOID app_UDP_thread_entry(ULONG thread_input)
//{
//	  HAL_GPIO_TogglePin(LED1_GREEN_GPIO_Port, LED1_GREEN_Pin);
//
//  UINT ret;
//  NX_PACKET *data_packet;
//
//  UINT pkt_number = 0;	/* packet number */
//  UINT offset = 0;
//  UINT packet_size = 1472;  /* Maximum safe packet size for Ethernet without fragmentation */
//  UINT header_size = sizeof(UINT);  /* Size of the packet number header */
//
//  /* create the UDP socket */
//  ret = nx_udp_socket_create(&NetXDuoEthIpInstance, &UDPSocket, "UDP Client Socket", NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, QUEUE_MAX_SIZE);
//  if (ret != NX_SUCCESS)
//  {
//    Error_Handler();
//  }
//
//  /* bind UDP socket to the DEFAULT PORT */
//  ret = nx_udp_socket_bind(&UDPSocket, DEFAULT_PORT, TX_WAIT_FOREVER);
//  if (ret != NX_SUCCESS)
//  {
//    Error_Handler();
//  }
//
//  while (offset < MEMORY_SIZE)
//  {
//    /* create the packet to send over the UDP socket */
//    ret = nx_packet_allocate(&NxAppPool, &data_packet, NX_UDP_PACKET, TX_WAIT_FOREVER);
//    if (ret != NX_SUCCESS)
//    {
//      Error_Handler();
//    }
//
//    /* Determine the size of the current packet */
//    ULONG current_packet_size = (MEMORY_SIZE - offset) > (packet_size - header_size) ? (packet_size - header_size) : (MEMORY_SIZE - offset);
//
//    /* Append packet number to the packet */
//    ret = nx_packet_data_append(data_packet, (VOID *) &pkt_number, header_size, &NxAppPool, TX_WAIT_FOREVER);
//    if (ret != NX_SUCCESS)
//    {
//      Error_Handler();
//    }
//
//    /* Append data from the memory area to the packet */
//    ret = nx_packet_data_append(data_packet, (VOID *)(memory_area + offset), current_packet_size, &NxAppPool, TX_WAIT_FOREVER);
//    if (ret != NX_SUCCESS)
//    {
//      Error_Handler();
//    }
//
//    /* Send the packet */
//    ret = nx_udp_socket_send(&UDPSocket, data_packet, UDP_SERVER_ADDRESS, UDP_SERVER_PORT);
//    if (ret != NX_SUCCESS)
//    {
//      Error_Handler();
//    }
//
//    /* Move the offset for the next packet */
//    offset += current_packet_size;
//	  /* Increase sequence number of the packets */
//    pkt_number++;
//
//
//
//
////    tx_thread_sleep(20);
//  }
//
//  /* unbind the socket and delete it */
//  nx_udp_socket_unbind(&UDPSocket);
//  nx_udp_socket_delete(&UDPSocket);
//
//
//
//  if (offset >= MEMORY_SIZE)
//  {
//    HAL_PWR_DisableWakeUpPin(PWR_WKUP1);
//    HAL_PWR_EnableWakeUpPin(PWR_WKUP1);
//    HAL_PWR_EnterSTANDBYMode();
//  }
//
//}



///* Send a UDP packet and wait for server acknowledge for all memory content */
//static VOID app_UDP_thread_entry(ULONG thread_input)
//{
//  HAL_GPIO_TogglePin(LED1_GREEN_GPIO_Port, LED1_GREEN_Pin);
//  UINT ret;
//  UCHAR data_buffer[512];
//  ULONG bytes_read;
//  NX_PACKET *server_packet;
//  NX_PACKET *data_packet;
//  UINT pkt_number = 0;
//
//  /* Memory offeset for reading */
//  ULONG offset = 0;
//
//  /* Maximum packet size MTU Ethernet - (Minumum IP Header + Udp Header) = 1500 - (20 + 8) */
//  ULONG packet_size = 1472;
//
//  /* Size of the packet number header */
//  UINT header_size = sizeof(UINT);
//
//  /* create the UDP socket */
//  ret = nx_udp_socket_create(&NetXDuoEthIpInstance, &UDPSocket, "UDP Client Socket", NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, QUEUE_MAX_SIZE);
//  if (ret != NX_SUCCESS)
//  {
//	  HAL_PWR_DisableWakeUpPin(PWR_WKUP1);
//	      HAL_PWR_EnableWakeUpPin(PWR_WKUP1);
//	      HAL_PWR_EnterSTANDBYMode();
//  }
//
//  /* bind UDP socket to the DEFAULT PORT */
//  ret = nx_udp_socket_bind(&UDPSocket, DEFAULT_PORT, TX_WAIT_FOREVER);
//  if (ret != NX_SUCCESS)
//  {
//	  HAL_PWR_DisableWakeUpPin(PWR_WKUP1);
//	      HAL_PWR_EnableWakeUpPin(PWR_WKUP1);
//	      HAL_PWR_EnterSTANDBYMode();
//  }
//
//  while (offset < MEMORY_SIZE)
//  {
//    TX_MEMSET(data_buffer, '\0', sizeof(data_buffer));
//
//    /* create the packet to send over the UDP socket */
//    ret = nx_packet_allocate(&NxAppPool, &data_packet, NX_UDP_PACKET, TX_WAIT_FOREVER);
//    if (ret != NX_SUCCESS)
//    {
//    	HAL_PWR_DisableWakeUpPin(PWR_WKUP1);
//    	    HAL_PWR_EnableWakeUpPin(PWR_WKUP1);
//    	    HAL_PWR_EnterSTANDBYMode();
//    }
//
//    /* Determine the size of the current packet */
//    ULONG current_packet_size = (MEMORY_SIZE - offset) > (packet_size - header_size) ? (packet_size - header_size) : (MEMORY_SIZE - offset);
//
//    /* Append packet sequence number to the packet */
//    ret = nx_packet_data_append(data_packet, (VOID *) &pkt_number, header_size, &NxAppPool, TX_WAIT_FOREVER);
//    if (ret != NX_SUCCESS)
//    {
//    	HAL_PWR_DisableWakeUpPin(PWR_WKUP1);
//    	    HAL_PWR_EnableWakeUpPin(PWR_WKUP1);
//    	    HAL_PWR_EnterSTANDBYMode();
//    }
//
//    /* Append data from the memory area to the packet */
//    ret = nx_packet_data_append(data_packet, (VOID *)(memory_area + offset), current_packet_size, &NxAppPool, TX_WAIT_FOREVER);
//    if (ret != NX_SUCCESS)
//    {
//    	HAL_PWR_DisableWakeUpPin(PWR_WKUP1);
//    	    HAL_PWR_EnableWakeUpPin(PWR_WKUP1);
//    	    HAL_PWR_EnterSTANDBYMode();
//    }
//
//    /* Send the packet */
//    ret = nx_udp_socket_send(&UDPSocket, data_packet, UDP_SERVER_ADDRESS, UDP_SERVER_PORT);
//    if (ret != NX_SUCCESS)
//    {
//    	HAL_PWR_DisableWakeUpPin(PWR_WKUP1);
//    	    HAL_PWR_EnableWakeUpPin(PWR_WKUP1);
//    	    HAL_PWR_EnterSTANDBYMode();
//    }
//
////    HAL_GPIO_TogglePin(LED1_GREEN_GPIO_Port, LED1_GREEN_Pin);
//
//    /* wait to receive response from the server */
//    ret = nx_udp_socket_receive(&UDPSocket, &server_packet, NX_APP_DEFAULT_TIMEOUT);
//    if (ret == NX_SUCCESS)
//    {
//      ULONG source_ip_address;
//      UINT source_port;
//
//      /* Get the server IP address and port */
//      nx_udp_source_extract(server_packet, &source_ip_address, &source_port);
//
//      /* Retrieve the data sent by the server */
//      nx_packet_data_retrieve(server_packet, data_buffer, &bytes_read);
//
//      /* Print the received data */
//      PRINT_DATA(source_ip_address, source_port, data_buffer);
//
//      /* Release the server packet */
//      nx_packet_release(server_packet);
//    }
//    else
//    {
//      /* Connection lost with the server, exit the loop */
//      break;
//    }
//
//    /* Move the offset for the next packet */
//    offset += current_packet_size;
//
//    /* Increase sequence number of the packets */
//    pkt_number++;
//  }
//
//  /* unbind the socket and delete it */
//  nx_udp_socket_unbind(&UDPSocket);
//  nx_udp_socket_delete(&UDPSocket);
//
//
//
//    printf("\n-------------------------------------\n\tSUCCESS : %lu / %lu Bytes sent\n-------------------------------------\n", (unsigned long)offset, (unsigned long)MEMORY_SIZE);
//    HAL_PWR_DisableWakeUpPin(PWR_WKUP1);
//    HAL_PWR_EnableWakeUpPin(PWR_WKUP1);
//    HAL_PWR_EnterSTANDBYMode();
//
//
//}

static VOID app_UDP_thread_entry(ULONG thread_input)
{
	tx_thread_resume(&AppUDPThread2);
  HAL_GPIO_TogglePin(LED1_GREEN_GPIO_Port, LED1_GREEN_Pin);
  UINT ret;

  NX_PACKET *server_packet;
  NX_PACKET *data_packet;

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
  ret = nx_udp_socket_bind(&UDPSocket, DEFAULT_PORT, TX_WAIT_FOREVER);
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

    /* Append packet number to the packet */
    ret = nx_packet_data_append(data_packet, (VOID *) &pkt_number, header_size, &NxAppPool, TX_WAIT_FOREVER);
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

    if (MEMORY_SIZE <= offset){

    }

  }

  /* wait to receive response from the server */
   // aspetta 2 secondi se non riceve nulla



}


/**
* @brief  Link thread entry
* @param thread_input: ULONG thread parameter
* @retval none
*/
static VOID app_link_thread_entry(ULONG thread_input)
{
	NX_PACKET *server_packet;
	 /* Size of the packet number header */
	  nx_udp_socket_create(&NetXDuoEthIpInstance, &UDPSocket2, "UDP Client Socket", NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, QUEUE_MAX_SIZE);

	  /* create the UDP socket */
	  nx_udp_socket_create(&NetXDuoEthIpInstance, &UDPSocket2, "UDP Client Socket", NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, QUEUE_MAX_SIZE);


	  /* bind UDP socket to the DEFAULT PORT */
	  nx_udp_socket_bind(&UDPSocket2, 6001, TX_WAIT_FOREVER);


	  /* wait to receive response from the server */
	  nx_udp_socket_receive(&UDPSocket2, &server_packet, NX_WAIT_FOREVER); // aspetta 2 secondi se non riceve nulla

		  HAL_GPIO_TogglePin(LED1_GREEN_GPIO_Port, LED1_GREEN_Pin);
	  	/* unbind the socket and delete it */
	  	nx_udp_socket_unbind(&UDPSocket2);
	  	nx_udp_socket_delete(&UDPSocket2);

	    HAL_PWR_DisableWakeUpPin(PWR_WKUP1);
	    HAL_PWR_EnableWakeUpPin(PWR_WKUP1);
	    HAL_PWR_EnterSTANDBYMode();
}


/* USER CODE END 1 */
