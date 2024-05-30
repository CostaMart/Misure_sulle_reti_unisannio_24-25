/**
  * da utilizzare per scrivere nei registri MMD del fisico ethernet.
  * originariamente scritta nel file /Drivers/BSP/Components/lan8742/lan8742.c, file che appare se si fa un progetto con NetxDuo attivo
  *
  * */


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
