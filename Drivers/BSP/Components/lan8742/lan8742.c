/**
  ******************************************************************************
  * @file    lan8742.c
  * @author  MCD Application Team
  * @brief   This file provides a set of functions needed to manage the LAN742
  *          PHY devices.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "lan8742.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Component
  * @{
  */

/** @defgroup LAN8742 LAN8742
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup LAN8742_Private_Defines LAN8742 Private Defines
  * @{
  */
#define LAN8742_SW_RESET_TO    ((uint32_t)500U)
#define LAN8742_INIT_TO        ((uint32_t)2000U)
#define LAN8742_MAX_DEV_ADDR   ((uint32_t)31U)
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/** @defgroup LAN8742_Private_Functions LAN8742 Private Functions
  * @{
  */

/**
  * @brief  Register IO functions to component object
  * @param  pObj: device object  of LAN8742_Object_t.
  * @param  ioctx: holds device IO functions.
  * @retval LAN8742_STATUS_OK  if OK
  *         LAN8742_STATUS_ERROR if missing mandatory function
  */
int32_t  LAN8742_RegisterBusIO(lan8742_Object_t *pObj, lan8742_IOCtx_t *ioctx)
{
  if(!pObj || !ioctx->ReadReg || !ioctx->WriteReg || !ioctx->GetTick)
  {
    return LAN8742_STATUS_ERROR;
  }

  pObj->IO.Init = ioctx->Init;
  pObj->IO.DeInit = ioctx->DeInit;
  pObj->IO.ReadReg = ioctx->ReadReg;
  pObj->IO.WriteReg = ioctx->WriteReg;
  pObj->IO.GetTick = ioctx->GetTick;

  return LAN8742_STATUS_OK;
}

/**
  * @brief  Initialize the lan8742 and configure the needed hardware resources
  * @param  pObj: device object LAN8742_Object_t.
  * @retval LAN8742_STATUS_OK  if OK
  *         LAN8742_STATUS_ADDRESS_ERROR if cannot find device address
  *         LAN8742_STATUS_READ_ERROR if cannot read register
  *         LAN8742_STATUS_WRITE_ERROR if cannot write to register
  *         LAN8742_STATUS_RESET_TIMEOUT if cannot perform a software reset
  */
 int32_t LAN8742_Init(lan8742_Object_t *pObj)
 {
   uint32_t tickstart = 0, regvalue = 0, addr = 0;
   int32_t status = LAN8742_STATUS_OK;

   if(pObj->Is_Initialized == 0)
   {
     if(pObj->IO.Init != 0)
     {
       /* GPIO and Clocks initialization */
       pObj->IO.Init();
     }

     /* for later check */
     pObj->DevAddr = LAN8742_MAX_DEV_ADDR + 1;

     /* Get the device address from special mode register */
     for(addr = 0; addr <= LAN8742_MAX_DEV_ADDR; addr ++)
     {
       if(pObj->IO.ReadReg(addr, LAN8742_SMR, &regvalue) < 0)
       {
         status = LAN8742_STATUS_READ_ERROR;
         /* Can't read from this device address
            continue with next address */
         continue;
       }

       if((regvalue & LAN8742_SMR_PHY_ADDR) == addr)
       {
         pObj->DevAddr = addr;
         status = LAN8742_STATUS_OK;
         break;
       }
     }

     if(pObj->DevAddr > LAN8742_MAX_DEV_ADDR)
     {
       status = LAN8742_STATUS_ADDRESS_ERROR;
     }

     /* if device address is matched */
     if(status == LAN8742_STATUS_OK)
     {
       /* set a software reset  */
       if(pObj->IO.WriteReg(pObj->DevAddr, LAN8742_BCR, LAN8742_BCR_SOFT_RESET) >= 0)
       {
         /* get software reset status */
         if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &regvalue) >= 0)
         {
           tickstart = pObj->IO.GetTick();

           /* wait until software reset is done or timeout occurred */
           while(regvalue & LAN8742_BCR_SOFT_RESET)
           {
             if((pObj->IO.GetTick() - tickstart) <= LAN8742_SW_RESET_TO)
             {
               if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &regvalue) < 0)
               {
                 status = LAN8742_STATUS_READ_ERROR;
                 break;
               }
             }
             else
             {
               status = LAN8742_STATUS_RESET_TIMEOUT;
               break;
             }
           }
         }
         else
         {
           status = LAN8742_STATUS_READ_ERROR;
         }
       }
       else
       {
         status = LAN8742_STATUS_WRITE_ERROR;
       }
     }
   }

   if(status == LAN8742_STATUS_OK)
   {
     tickstart =  pObj->IO.GetTick();

     /* Wait for 2s to perform initialization */
     while((pObj->IO.GetTick() - tickstart) <= LAN8742_INIT_TO)
     {
     }
     pObj->Is_Initialized = 1;
   }

   LAN8742_enable_wol(pObj);

   return status;
 }

/**
  * @brief  De-Initialize the lan8742 and it's hardware resources
  * @param  pObj: device object LAN8742_Object_t.
  * @retval None
  */
int32_t LAN8742_DeInit(lan8742_Object_t *pObj)
{
  if(pObj->Is_Initialized)
  {
    if(pObj->IO.DeInit != 0)
    {
      if(pObj->IO.DeInit() < 0)
      {
        return LAN8742_STATUS_ERROR;
      }
    }

    pObj->Is_Initialized = 0;
  }

  return LAN8742_STATUS_OK;
}

/**
  * @brief  Disable the LAN8742 power down mode.
  * @param  pObj: device object LAN8742_Object_t.
  * @retval LAN8742_STATUS_OK  if OK
  *         LAN8742_STATUS_READ_ERROR if cannot read register
  *         LAN8742_STATUS_WRITE_ERROR if cannot write to register
  */
int32_t LAN8742_DisablePowerDownMode(lan8742_Object_t *pObj)
{
  uint32_t readval = 0;
  int32_t status = LAN8742_STATUS_OK;

  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &readval) >= 0)
  {
    readval &= ~LAN8742_BCR_POWER_DOWN;

    /* Apply configuration */
    if(pObj->IO.WriteReg(pObj->DevAddr, LAN8742_BCR, readval) < 0)
    {
      status =  LAN8742_STATUS_WRITE_ERROR;
    }
  }
  else
  {
    status = LAN8742_STATUS_READ_ERROR;
  }

  return status;
}

/**
  * @brief  Enable the LAN8742 power down mode.
  * @param  pObj: device object LAN8742_Object_t.
  * @retval LAN8742_STATUS_OK  if OK
  *         LAN8742_STATUS_READ_ERROR if cannot read register
  *         LAN8742_STATUS_WRITE_ERROR if cannot write to register
  */
int32_t LAN8742_EnablePowerDownMode(lan8742_Object_t *pObj)
{
  uint32_t readval = 0;
  int32_t status = LAN8742_STATUS_OK;

  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &readval) >= 0)
  {
    readval |= LAN8742_BCR_POWER_DOWN;

    /* Apply configuration */
    if(pObj->IO.WriteReg(pObj->DevAddr, LAN8742_BCR, readval) < 0)
    {
      status =  LAN8742_STATUS_WRITE_ERROR;
    }
  }
  else
  {
    status = LAN8742_STATUS_READ_ERROR;
  }

  return status;
}

/**
  * @brief  Start the auto negotiation process.
  * @param  pObj: device object LAN8742_Object_t.
  * @retval LAN8742_STATUS_OK  if OK
  *         LAN8742_STATUS_READ_ERROR if cannot read register
  *         LAN8742_STATUS_WRITE_ERROR if cannot write to register
  */
int32_t LAN8742_StartAutoNego(lan8742_Object_t *pObj)
{
  uint32_t readval = 0;
  int32_t status = LAN8742_STATUS_OK;

  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &readval) >= 0)
  {
    readval |= LAN8742_BCR_AUTONEGO_EN;

    /* Apply configuration */
    if(pObj->IO.WriteReg(pObj->DevAddr, LAN8742_BCR, readval) < 0)
    {
      status =  LAN8742_STATUS_WRITE_ERROR;
    }
  }
  else
  {
    status = LAN8742_STATUS_READ_ERROR;
  }

  return status;
}

/**
  * @brief  Get the link state of LAN8742 device.
  * @param  pObj: Pointer to device object.
  * @param  pLinkState: Pointer to link state
  * @retval LAN8742_STATUS_LINK_DOWN  if link is down
  *         LAN8742_STATUS_AUTONEGO_NOTDONE if Auto nego not completed
  *         LAN8742_STATUS_100MBITS_FULLDUPLEX if 100Mb/s FD
  *         LAN8742_STATUS_100MBITS_HALFDUPLEX if 100Mb/s HD
  *         LAN8742_STATUS_10MBITS_FULLDUPLEX  if 10Mb/s FD
  *         LAN8742_STATUS_10MBITS_HALFDUPLEX  if 10Mb/s HD
  *         LAN8742_STATUS_READ_ERROR if cannot read register
  *         LAN8742_STATUS_WRITE_ERROR if cannot write to register
  */
int32_t LAN8742_GetLinkState(lan8742_Object_t *pObj)
{
  uint32_t readval = 0;

  /* Read Status register  */
  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BSR, &readval) < 0)
  {
    return LAN8742_STATUS_READ_ERROR;
  }

  /* Read Status register again */
  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BSR, &readval) < 0)
  {
    return LAN8742_STATUS_READ_ERROR;
  }

  if((readval & LAN8742_BSR_LINK_STATUS) == 0)
  {
    /* Return Link Down status */
    return LAN8742_STATUS_LINK_DOWN;
  }

  /* Check Auto negotiation */
  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &readval) < 0)
  {
    return LAN8742_STATUS_READ_ERROR;
  }

  if((readval & LAN8742_BCR_AUTONEGO_EN) != LAN8742_BCR_AUTONEGO_EN)
  {
    if(((readval & LAN8742_BCR_SPEED_SELECT) == LAN8742_BCR_SPEED_SELECT) && ((readval & LAN8742_BCR_DUPLEX_MODE) == LAN8742_BCR_DUPLEX_MODE))
    {
      return LAN8742_STATUS_100MBITS_FULLDUPLEX;
    }
    else if ((readval & LAN8742_BCR_SPEED_SELECT) == LAN8742_BCR_SPEED_SELECT)
    {
      return LAN8742_STATUS_100MBITS_HALFDUPLEX;
    }
    else if ((readval & LAN8742_BCR_DUPLEX_MODE) == LAN8742_BCR_DUPLEX_MODE)
    {
      return LAN8742_STATUS_10MBITS_FULLDUPLEX;
    }
    else
    {
      return LAN8742_STATUS_10MBITS_HALFDUPLEX;
    }
  }
  else /* Auto Nego enabled */
  {
    if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_PHYSCSR, &readval) < 0)
    {
      return LAN8742_STATUS_READ_ERROR;
    }

    /* Check if auto nego not done */
    if((readval & LAN8742_PHYSCSR_AUTONEGO_DONE) == 0)
    {
      return LAN8742_STATUS_AUTONEGO_NOTDONE;
    }

    if((readval & LAN8742_PHYSCSR_HCDSPEEDMASK) == LAN8742_PHYSCSR_100BTX_FD)
    {
      return LAN8742_STATUS_100MBITS_FULLDUPLEX;
    }
    else if ((readval & LAN8742_PHYSCSR_HCDSPEEDMASK) == LAN8742_PHYSCSR_100BTX_HD)
    {
      return LAN8742_STATUS_100MBITS_HALFDUPLEX;
    }
    else if ((readval & LAN8742_PHYSCSR_HCDSPEEDMASK) == LAN8742_PHYSCSR_10BT_FD)
    {
      return LAN8742_STATUS_10MBITS_FULLDUPLEX;
    }
    else
    {
      return LAN8742_STATUS_10MBITS_HALFDUPLEX;
    }
  }
}

/**
  * @brief  Set the link state of LAN8742 device.
  * @param  pObj: Pointer to device object.
  * @param  pLinkState: link state can be one of the following
  *         LAN8742_STATUS_100MBITS_FULLDUPLEX if 100Mb/s FD
  *         LAN8742_STATUS_100MBITS_HALFDUPLEX if 100Mb/s HD
  *         LAN8742_STATUS_10MBITS_FULLDUPLEX  if 10Mb/s FD
  *         LAN8742_STATUS_10MBITS_HALFDUPLEX  if 10Mb/s HD
  * @retval LAN8742_STATUS_OK  if OK
  *         LAN8742_STATUS_ERROR  if parameter error
  *         LAN8742_STATUS_READ_ERROR if cannot read register
  *         LAN8742_STATUS_WRITE_ERROR if cannot write to register
  */
int32_t LAN8742_SetLinkState(lan8742_Object_t *pObj, uint32_t LinkState)
{
  uint32_t bcrvalue = 0;
  int32_t status = LAN8742_STATUS_OK;

  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &bcrvalue) >= 0)
  {
    /* Disable link config (Auto nego, speed and duplex) */
    bcrvalue &= ~(LAN8742_BCR_AUTONEGO_EN | LAN8742_BCR_SPEED_SELECT | LAN8742_BCR_DUPLEX_MODE);

    if(LinkState == LAN8742_STATUS_100MBITS_FULLDUPLEX)
    {
      bcrvalue |= (LAN8742_BCR_SPEED_SELECT | LAN8742_BCR_DUPLEX_MODE);
    }
    else if (LinkState == LAN8742_STATUS_100MBITS_HALFDUPLEX)
    {
      bcrvalue |= LAN8742_BCR_SPEED_SELECT;
    }
    else if (LinkState == LAN8742_STATUS_10MBITS_FULLDUPLEX)
    {
      bcrvalue |= LAN8742_BCR_DUPLEX_MODE;
    }
    else
    {
      /* Wrong link status parameter */
      status = LAN8742_STATUS_ERROR;
    }
  }
  else
  {
    status = LAN8742_STATUS_READ_ERROR;
  }

  if(status == LAN8742_STATUS_OK)
  {
    /* Apply configuration */
    if(pObj->IO.WriteReg(pObj->DevAddr, LAN8742_BCR, bcrvalue) < 0)
    {
      status = LAN8742_STATUS_WRITE_ERROR;
    }
  }

  return status;
}

/**
  * @brief  Enable loopback mode.
  * @param  pObj: Pointer to device object.
  * @retval LAN8742_STATUS_OK  if OK
  *         LAN8742_STATUS_READ_ERROR if cannot read register
  *         LAN8742_STATUS_WRITE_ERROR if cannot write to register
  */
int32_t LAN8742_EnableLoopbackMode(lan8742_Object_t *pObj)
{
  uint32_t readval = 0;
  int32_t status = LAN8742_STATUS_OK;

  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &readval) >= 0)
  {
    readval |= LAN8742_BCR_LOOPBACK;

    /* Apply configuration */
    if(pObj->IO.WriteReg(pObj->DevAddr, LAN8742_BCR, readval) < 0)
    {
      status = LAN8742_STATUS_WRITE_ERROR;
    }
  }
  else
  {
    status = LAN8742_STATUS_READ_ERROR;
  }

  return status;
}

/**
  * @brief  Disable loopback mode.
  * @param  pObj: Pointer to device object.
  * @retval LAN8742_STATUS_OK  if OK
  *         LAN8742_STATUS_READ_ERROR if cannot read register
  *         LAN8742_STATUS_WRITE_ERROR if cannot write to register
  */
int32_t LAN8742_DisableLoopbackMode(lan8742_Object_t *pObj)
{
  uint32_t readval = 0;
  int32_t status = LAN8742_STATUS_OK;

  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &readval) >= 0)
  {
    readval &= ~LAN8742_BCR_LOOPBACK;

    /* Apply configuration */
    if(pObj->IO.WriteReg(pObj->DevAddr, LAN8742_BCR, readval) < 0)
    {
      status =  LAN8742_STATUS_WRITE_ERROR;
    }
  }
  else
  {
    status = LAN8742_STATUS_READ_ERROR;
  }

  return status;
}

/**
  * @brief  Enable IT source.
  * @param  pObj: Pointer to device object.
  * @param  Interrupt: IT source to be enabled
  *         should be a value or a combination of the following:
  *         LAN8742_WOL_IT
  *         LAN8742_ENERGYON_IT
  *         LAN8742_AUTONEGO_COMPLETE_IT
  *         LAN8742_REMOTE_FAULT_IT
  *         LAN8742_LINK_DOWN_IT
  *         LAN8742_AUTONEGO_LP_ACK_IT
  *         LAN8742_PARALLEL_DETECTION_FAULT_IT
  *         LAN8742_AUTONEGO_PAGE_RECEIVED_IT
  * @retval LAN8742_STATUS_OK  if OK
  *         LAN8742_STATUS_READ_ERROR if cannot read register
  *         LAN8742_STATUS_WRITE_ERROR if cannot write to register
  */
int32_t LAN8742_EnableIT(lan8742_Object_t *pObj, uint32_t Interrupt)
{
  uint32_t readval = 0;
  int32_t status = LAN8742_STATUS_OK;

  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_IMR, &readval) >= 0)
  {
    readval |= Interrupt;

    /* Apply configuration */
    if(pObj->IO.WriteReg(pObj->DevAddr, LAN8742_IMR, readval) < 0)
    {
      status =  LAN8742_STATUS_WRITE_ERROR;
    }
  }
  else
  {
    status = LAN8742_STATUS_READ_ERROR;
  }

  return status;
}

/**
  * @brief  Disable IT source.
  * @param  pObj: Pointer to device object.
  * @param  Interrupt: IT source to be disabled
  *         should be a value or a combination of the following:
  *         LAN8742_WOL_IT
  *         LAN8742_ENERGYON_IT
  *         LAN8742_AUTONEGO_COMPLETE_IT
  *         LAN8742_REMOTE_FAULT_IT
  *         LAN8742_LINK_DOWN_IT
  *         LAN8742_AUTONEGO_LP_ACK_IT
  *         LAN8742_PARALLEL_DETECTION_FAULT_IT
  *         LAN8742_AUTONEGO_PAGE_RECEIVED_IT
  * @retval LAN8742_STATUS_OK  if OK
  *         LAN8742_STATUS_READ_ERROR if cannot read register
  *         LAN8742_STATUS_WRITE_ERROR if cannot write to register
  */
int32_t LAN8742_DisableIT(lan8742_Object_t *pObj, uint32_t Interrupt)
{
  uint32_t readval = 0;
  int32_t status = LAN8742_STATUS_OK;

  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_IMR, &readval) >= 0)
  {
    readval &= ~Interrupt;

    /* Apply configuration */
    if(pObj->IO.WriteReg(pObj->DevAddr, LAN8742_IMR, readval) < 0)
    {
      status = LAN8742_STATUS_WRITE_ERROR;
    }
  }
  else
  {
    status = LAN8742_STATUS_READ_ERROR;
  }

  return status;
}

/**
  * @brief  Clear IT flag.
  * @param  pObj: Pointer to device object.
  * @param  Interrupt: IT flag to be cleared
  *         should be a value or a combination of the following:
  *         LAN8742_WOL_IT
  *         LAN8742_ENERGYON_IT
  *         LAN8742_AUTONEGO_COMPLETE_IT
  *         LAN8742_REMOTE_FAULT_IT
  *         LAN8742_LINK_DOWN_IT
  *         LAN8742_AUTONEGO_LP_ACK_IT
  *         LAN8742_PARALLEL_DETECTION_FAULT_IT
  *         LAN8742_AUTONEGO_PAGE_RECEIVED_IT
  * @retval LAN8742_STATUS_OK  if OK
  *         LAN8742_STATUS_READ_ERROR if cannot read register
  */
int32_t  LAN8742_ClearIT(lan8742_Object_t *pObj, uint32_t Interrupt)
{
  uint32_t readval = 0;
  int32_t status = LAN8742_STATUS_OK;

  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_ISFR, &readval) < 0)
  {
    status =  LAN8742_STATUS_READ_ERROR;
  }

  return status;
}

/**
  * @brief  Get IT Flag status.
  * @param  pObj: Pointer to device object.
  * @param  Interrupt: IT Flag to be checked,
  *         should be a value or a combination of the following:
  *         LAN8742_WOL_IT
  *         LAN8742_ENERGYON_IT
  *         LAN8742_AUTONEGO_COMPLETE_IT
  *         LAN8742_REMOTE_FAULT_IT
  *         LAN8742_LINK_DOWN_IT
  *         LAN8742_AUTONEGO_LP_ACK_IT
  *         LAN8742_PARALLEL_DETECTION_FAULT_IT
  *         LAN8742_AUTONEGO_PAGE_RECEIVED_IT
  * @retval 1 IT flag is SET
  *         0 IT flag is RESET
  *         LAN8742_STATUS_READ_ERROR if cannot read register
  */
int32_t LAN8742_GetITStatus(lan8742_Object_t *pObj, uint32_t Interrupt)
{
  uint32_t readval = 0;
  int32_t status = 0;

  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_ISFR, &readval) >= 0)
  {
    status = ((readval & Interrupt) == Interrupt);
  }
  else
  {
    status = LAN8742_STATUS_READ_ERROR;
  }

  return status;
}


/**
  * da utilizzare per scrivere nei registri MMD del fisico ethernet.
  * originariamente scritta nel file /Drivers/BSP/Components/lan8742/lan8742.c, file che appare se si fa un progetto con NetxDuo attivo
  * IMPORTANTE: nella funzione di init del fisico c'è un' attesa di 2 secondi durante la quale il sistema operativo riprende il controllo e fa cose strane, potrebbe essere opportuno levarla
  * */

/** @defgroup LAN8742_Registers_Mapping LAN8742 Registers Mapping
  * @{
  */
#define LAN8742_PHY_RX_ADDRA 		((uint16_t)0x8061U)
#define LAN8742_PHY_RX_ADDRB 		((uint16_t)0x8062U)
#define LAN8742_PHY_RX_ADDRC 		((uint16_t)0x8063U)
#define LAN8742_PHY_MCFGR			((uint16_t)0x8064U)
#define LAN8742_PHY_MMD_WOL_WUCSR	((uint16_t)0x8010U)


#define LAN8742_PME_ENABLE	((uint16_t)0x4000U)		/* Bit 14 to 1 to enable PME on Led1 (YELLOW) */
#define LAN8742_SELF_CLEAR	((uint16_t)0x0200U)		/* Bit 9 to 1 to enable self clear */
#define LAN8742_WOL_ENABLE	((uint16_t)0x0002U)		/* Bit 1 to 1 to enable wake on lan with magic packet */
#define LAN8742_MPR_RESET	((uint16_t)0x0020U)		/* The MAC sets this bit upon receiving a valid Magic Packet */

void LAN8742_enable_wol(lan8742_Object_t *pObj){
 	 uint16_t val;

 	 val = LAN8742_PME_ENABLE;
 	 val |= LAN8742_SELF_CLEAR;
 	 val |= LAN8742_WOL_ENABLE;
 	 val |= LAN8742_MPR_RESET;

     // passo 1 -- settiamo i bit 15-14 a 01 per abilitare PME sul led1
 	// abilitiamo anche il self clear ponendo il bit 9 a 1
 	// alzo il secondo bit a 1 per abilitare il wake on lan tramite magic packet
 	// alzo il bit Magic Packet Received (MPR) (bit 5)  a 1 per sollecitarne il reset
     // valore binario finale: 0100 0010 0010 0010
 	// hex: 0x4222
 	 LAN8742_write_mmd(pObj, LAN8742_PHY_MMD_WOL_WUCSR, val);


 	 val = 0x1000;
 	// passo 1 impostiamo il tempo di reset per il self clear del bit PME
 	 LAN8742_write_mmd(pObj, LAN8742_PHY_MCFGR, val);

 	 // imposto i registri ADDR per riconoscere il MAC nel magic packet
 	 // assumento che il MAC sia 00:80:E1:00:00:00

 	 // registro A:
 	 uint16_t A = 0x0000;
 	 LAN8742_write_mmd(pObj, LAN8742_PHY_RX_ADDRA, A);
 	 // registro B:
 	 uint16_t B = 0x00E1;
 	 LAN8742_write_mmd(pObj, LAN8742_PHY_RX_ADDRB, B);
 	 // registro C:
 	 uint16_t C = 0x8000;
 	 LAN8742_write_mmd(pObj, LAN8742_PHY_RX_ADDRC, C);
  }




/**
  * @brief write in MMD register
  * @param pObj: device object LAN8742_Object_t.
  * @param target: address to write in.
  * @param content: content to write in target register
  *
  * */
 void LAN8742_write_mmd(lan8742_Object_t *pObj, uint16_t target, uint16_t content){

	 // Imposta funzione: bit 14-15 a 00 (MMD_ADDRESS)
	 uint16_t val = 0x00;

	 // imposto l'indirizzo del dispositivo (bit 0-4) a 00011 (3)
	 val |= 0x03;

	 // scrivo nel registro Access control register
	 pObj->IO.WriteReg(pObj->DevAddr, LAN8742_MMDACR, val);

	 // scrivo nel registro MMD ACCESS ADDRESS/DATA REGISTER l'indirizzo del registro target della scrittura
	 pObj->IO.WriteReg(pObj->DevAddr, LAN8742_MMDAADR, target);

	 //------------ scrittura inizializzata --------------

	 //imposta funzione: bit 14-15 a 01
	 val = LAN8742_MMDACR_MMD_FUNCTION_DATA; // 0x4000U
	 val |= 0x03;

	 // scrivo nel registro Access control register
	 pObj->IO.WriteReg(pObj->DevAddr, LAN8742_MMDACR, val);

	 // scrivo nel registro MMD ACCESS ADDRESS/DATA REGISTER i dati da inviare
	 pObj->IO.WriteReg(pObj->DevAddr, LAN8742_MMDAADR, content);
 }

 /**
  * @brief read from MMD register
  * @param pObj: device object LAN8742_Object_t.
  * @param target: address to read from.
  * @param returnedVal: where to store the read data.
  */
 void LAN8742_read_mmd(lan8742_Object_t *pObj, uint16_t target, uint16_t* returnedVal){

	 // Imposta funzione: bit 14-15 a 00 (MMD_ADDRESS)
	 uint16_t val = 0x00;

	 // imposto l'indirizzo del dispositivo (bit 0-4) a 00011 (3)
	 val |= 0x03;

	 // scrivo nel registro Access control register
	 pObj->IO.WriteReg(pObj->DevAddr, LAN8742_MMDACR, val);

	 // scrivo nel registro MMD ACCESS ADDRESS/DATA REGISTER l'indirizzo del registro target della lettura
	 pObj->IO.WriteReg(pObj->DevAddr, LAN8742_MMDAADR, target);

	 //------------ lettura inizializzata --------------

	 //imposta funzione: bit 14-15 a 01
	 val = LAN8742_MMDACR_MMD_FUNCTION_DATA; // 0x4000U
	 val |= 0x03;
	 // scrivo nel registro Access control register
	 pObj->IO.WriteReg(pObj->DevAddr, LAN8742_MMDACR, val);

	 // leggo dal registro MMD ACCESS ADDRESS/DATA REGISTER i dati richiesti
	 pObj->IO.ReadReg(pObj->DevAddr, LAN8742_MMDAADR, returnedVal);
 }

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
