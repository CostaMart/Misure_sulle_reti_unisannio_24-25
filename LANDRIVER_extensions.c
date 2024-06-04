/**
  * da utilizzare per scrivere nei registri MMD del fisico ethernet.
  * originariamente scritta nel file /Drivers/BSP/Components/lan8742/lan8742.c, file che appare se si fa un progetto con NetxDuo attivo
  * IMPORTANTE: nella funzione di init del fisico c'è un' attesa di 2 secondi durante la quale il sistema operativo riprende il controllo e fa cose strane, potrebbe essere opportuno levarla
  * */

#define MII_LAN874X_PHY_RX_ADDRA ((uint16_t)0x8061U)
#define MII_LAN874X_PHY_RX_ADDRB ((uint16_t)0x8062U)
#define MII_LAN874X_PHY_RX_ADDRC ((uint16_t)0x8063U)
#define MII_LAN874X_PHY_MCFGR			((uint16_t)0x8064U)
#define MII_LAN874X_PHY_MMD_WOL_WUCSR		((uint16_t)0x8010U)

void LAN8742_enable_wol(lan8742_Object_t *pObj){
 	 uint16_t val = 0x4222;

     // passo 1 -- settiamo i bit 01 per abilitare PME sul led1
 	// abilitiamo anche il self clear ponendo il bit 9 a 1
 	// alzo il secondo bit a 1 per abilitare il wake on lan tramite magic packet
 	// alzo il bit Magic Packet Received (MPR) (bit 5)  a 1 per sollecitarne il reset
     // valore binario finale: 0100001000100010
 	// hex: 0x4222
 	 LAN8742_write_mmd(pObj, MII_LAN874X_PHY_MMD_WOL_WUCSR, val);


 	 val = 0x1000;
 	// passo 1 impostiamo il tempo di reset per il self clear del bit PME
 	 LAN8742_write_mmd(pObj, MII_LAN874X_PHY_MCFGR, val);

 	 // imposto i registri ADDR per riconoscere il MAC nel magic packet
 	 // assumento che il MAC sia 00:80:E1:00:00:00

 	 // registro A:
 	 uint16_t A = 0x0000;
 	 LAN8742_write_mmd(pObj, MII_LAN874X_PHY_RX_ADDRA, A);
 	 // registro B:
 	 uint16_t B = 0x00E1;
 	 LAN8742_write_mmd(pObj, MII_LAN874X_PHY_RX_ADDRB, B);
 	 // registro C:
 	 uint16_t C = 0x8000;
 	 LAN8742_write_mmd(pObj, MII_LAN874X_PHY_RX_ADDRC, C);
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
