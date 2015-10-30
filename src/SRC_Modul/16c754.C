    //------------------------------------------------------------------------   
    #include   "AT16C554.H"   
    //--------------------------------------------------------------------------------------------------   
    code        Uint    PORTAddrTABLE[PortNumber]={STARTADDR_SERIAL1,   
                                                   STARTADDR_SERIAL2,   
                                                   STARTADDR_SERIAL3,   
                                                   STARTADDR_SERIAL4};          //ƒ°œÍ∆€œÍ≈ËÊ«„√ŸÙ¡˝   
       
       
    xdata       TCommHandle     CommDevice[PortNumber];   
    sbit        reset_16c554 =  P1^4;                                           //ST16C554 »ƒﬁÀœËÊ÷ﬂÔ   
    //---------------------------------------------------------------------------------------------------   
    //  Define Private Faction   
    //-----------------------------------------------------------------------------------------------------   
       
    bool ReadPortRecBuf(uchar Port,Uchar *Byte);   
    bool WritePortSendBuf(uchar Port,Uchar Byte);   
    void Port_open(Uchar PortID,Uint Baud);   
       
    //-------------------------------------------------------------------------   
    //  Define Public Faction   
    //-------------------------------------------------------------------------   
       
    bool Init_16c554(void);   
    void Port_open(Uchar PortID,Uint Baud);   
    //void Port_open(Uchar PortID,int Baud,Uchar DataBitNum,Uchar Parit,Uchar Stop);   
    void Port_close(Uchar PortID);   
    int  Port_Receive(Uchar PortID,void *Buffer,int Count);   
    int  Port_Send(Uchar PortID,void *Buffer,int Count);   
       
       
       
    //---------------------------------------------------------------------------------------------   
    //  Public Faction Code   
    //---------------------------------------------------------------------------------------------   
       
       
       
       
    //---------------------------------------------------------------------------------------------   
    //  √æ⁄ÃÀèST16C554   
    //---------------------------------------------------------------------------------------------   
    bool Init_16c554(void)   
    {   
    uchar xdata *ptr;   
    uchar i=Init_16c554_Count;   
    bool Result=false;   
       
    while(--i)                                                  // »ƒﬁÀ10ƒﬁ¬À√Ÿ…≤Åç‰ê«≈ÀË√≥ƒ˝   
        {   
          _nop_();   
          reset_16c554 = FREE_RESET;   
          ms_delay(5);   
          reset_16c554 = RESET;                                 // »ƒﬁÀ16C554   
          ms_delay(10);                                         // ·„⁄¡1 —”˚   
          reset_16c554 = FREE_RESET;   
          ms_delay(10);   
          ptr = STARTADDR_SERIAL1+SPR;                          // ›£≈–1—È⁄¡Ã‘ƒˆ÷ø   
          *ptr = 0xaa;                                          // ‡ƒ¬Ú⁄‰Ê≈≈Õ—È⁄¡Ã‘ƒˆ÷ø   
          _nop_();   
          _nop_();   
          if(*ptr == 0xaa)    
            {   
               Result=true;   
               break;   
            };//end if   
        };//end while   
    for(i=0;i<PortNumber;i++) CommDevice[i].RAMAddress_Map=PORTAddrTABLE[i];   
       
    return Result;   
    }   
    //---------------------------------------------------------------------------------------------   
    //    ƒêœåƒ°œÍ       
    //---------------------------------------------------------------------------------------------   
    void Port_open(Uchar PortID,Uint Baud)   
    {   
    uchar xdata *ptr;   
       
    ptr = CommDevice[PortID].RAMAddress_Map;                                                // ÿ®∆€œÍ≈‘Àö≈ËÊ«   
    *(ptr+LCR) = LCR_DIV_EN;                                                                // ‰⁄‡˝«Ä€›¬£‹Ë“⁄   
    *(ptr+DLM) = (uchar)(Baud>>8);   
    *(ptr+DLL) = (uchar)Baud;                                                               // Ÿ¯Ê”‘çÿﬁ¬£‹Ë“⁄ﬁå19200   
    *(ptr+LCR) = LCR_WORD_LEN8|LCR_STOP_LEN1;                                               // Ÿ¯Ê”ÁÊ√™ﬁå8Åç1»≥›ÅÊ…ﬁÀÅçﬁÓ‡Å·˘ﬁÀ   
    *(ptr+FCR) = FIFO_EN|FIFO_TRI_LEVEL_1|FIFO_CLEAR_RCVR|FIFO_CLEAR_XMIT|FIFO_DMA_MODE;    // Ÿ¯Ê”FIFO œËÊ÷Ã‘ƒˆ÷ø   
    *(ptr+IER) = IER_TRANSMITHOLD_INT|IER_RECEIVEHOLD_INT;                                  // Ÿ¯Ê”Ê‡∆ﬂ÷—¡ﬁÃ‘ƒˆ÷ø   
    *(ptr+MCR) = MCR_INTX_EN;                                                               // ‰⁄‡˝Ê‡∆ﬂ‚ßÕ’⁄Ù√≥   
       
    if(PortID==0)   EX0 = true;                         // œå›Ú¬œÊ‡∆ﬂ0(›£≈–1Ê‡∆ﬂ)   
    if(PortID==1)   EX1 = true;                         // œå›Ú¬œÊ‡∆ﬂ1(›£≈–2Ê‡∆ﬂ)   
    if(PortID==2)   EX2 = true;                         // œå›Ú¬œÊ‡∆ﬂ2(›£≈–3Ê‡∆ﬂ)   
    if(PortID==3)   EX3 = true;                         // œå›Ú¬œÊ‡∆ﬂ3(›£≈–4Ê‡∆ﬂ)   
       
    CommDevice[PortID].CommReceive_Head=0;   
    CommDevice[PortID].CommReceive_Trail=0;   
    CommDevice[PortID].CommSend_Head=0;   
    CommDevice[PortID].CommSend_Trail=0;   
    CommDevice[PortID].Comm_Stu=SERIAL_STU_FREE;   
    }   
    //---------------------------------------------------------------------------------------------   
    //       …Ë¡ÂÊ»∆£≈‘∆€œÍ   
    //---------------------------------------------------------------------------------------------   
    /*void Port_close(Uchar PortID)  
    {  
    uchar xdata *ptr;  
      
    ptr = CommDevice[PortID].RAMAddress_Map;            // ÿ®∆€œÍ≈‘Àö≈ËÊ«  
    *(ptr+LCR) = LCR_DIV_Dis;                           // ÕûÊ…«Ä€›¬£‹Ë“⁄  
    *(ptr+MCR) = MCR_INTX_Dis;                          // ‰⁄‡˝Ê‡∆ﬂ‚ßÕ’⁄Ù√≥  
      
    CommDevice[PortID].CommReceive_Head=0;  
    CommDevice[PortID].CommReceive_Trail=0;  
    CommDevice[PortID].CommSend_Head=0;  
    CommDevice[PortID].CommSend_Trail=0;  
    CommDevice[PortID].Comm_Stu=SERIAL_STU_FREE;  
      
    if(PortID==0)   EX0 = false;                        // …Ë›Ú¬œÊ‡∆ﬂ0(›£≈–1Ê‡∆ﬂ)  
    if(PortID==1)   EX1 = false;                        // …Ë›Ú¬œÊ‡∆ﬂ1(›£≈–2Ê‡∆ﬂ)  
    if(PortID==2)   EX2 = false;                        // …Ë›Ú¬œÊ‡∆ﬂ2(›£≈–3Ê‡∆ﬂ)  
    if(PortID==3)   EX3 = false;                        // …Ë›Ú¬œÊ‡∆ﬂ3(›£≈–4Ê‡∆ﬂ)  
    }*/   
    //--------------------------------------------------------------------------------------------------   
      //…≤  ‘ÏÅ ƒ„ƒ°œÍÊ‡Õ„⁄Â⁄ßŒÌ   
      //«≈ÀËÊ≈Å ƒ„ƒ°œÍÊ‡Õ„⁄Â≈Õ≈‘⁄ßŒÌ»≥⁄ß   
      //¬ﬁ  ⁄ßÅ    
      //        BufferÅ Õ„⁄Â⁄ß≈‘À √ı◊º   
      //        CountÅ  ﬂ˚ƒ„ƒ°œÍÀ √ı◊ºÊ‡Õ„⁄Â⁄ßŒÌ»≥⁄ß   
    //--------------------------------------------------------------------------------------------------   
    /*int Port_Receive(uchar Port,register void *Buffer,int Count)  
    {  
    Uchar Size=0;  
      
    if(Port>PortNumber-1)  return Size;  
      
    while((Size<Count)&&(ReadPortRecBuf(Port,Buffer)))  
         {  
           Size++;  
           Buffer=Buffer+1;  
         };//end while   
    return Size;  
    }*/   
    //--------------------------------------------------------------------------------------------------   
      //…≤  ‘ÏÅ ›öƒ°œÍ«Ä€›⁄ßŒÌ   
      //«≈ÀËÊ≈Å ‚·«Ä€›≈‘⁄ßŒÌ»≥⁄ß   
      //¬ﬁ  ⁄ßÅ    
      //        BufferÅ «Ä€›⁄ßŒÌ≈‘À √ı◊º   
      //        CountÅ  ﬂ˚ƒ„ƒ°œÍ«Ä€›⁄ßŒÌ≈‘»≥⁄ß   
    //--------------------------------------------------------------------------------------------------   
    int Port_Send(uchar Port,register void *Buffer,int Count)   
    {   
    uchar           i=0;   
    Uchar           Size=0;   
    uchar xdata     *ptr;   
       
    if(Port>PortNumber-1)  return Size;   
       
    ptr = CommDevice[Port].RAMAddress_Map;                                                                          // ÿ®∆€œÍÀö≈ËÊ«   
       
    while(Size<Count)   
         {   
            if(WritePortSendBuf(Port,*((Uchar *)Buffer)))   
              {   
                Size=Size+1;   
                Buffer=Buffer+1;   
              };//end if   
            if(CommDevice[Port].Comm_Stu==SERIAL_STU_FREE)   
              {   
                for(i=0;(i<FIFO_SIZE)&&(CommDevice[Port].CommSend_Head!=CommDevice[Port].CommSend_Trail);i++)   
                    {      
                      CommDevice[Port].CommSend_Trail=(CommDevice[Port].CommSend_Trail+1)%CommMaxSendBuffer;   
                      *(ptr+THR) = CommDevice[Port].CommSendBuffer[CommDevice[Port].CommSend_Trail];                // «Ä€›⁄ßŒÌ   
                    };//end for   
                CommDevice[Port].Comm_Stu = SERIAL_STU_BUSY;   
              };//end if   
         };//end while   
    return Size;   
    }   
    //----------------------------------------------------------------------------------------------------   
       
       
       
    //----------------------------------------------------------------------------------------------------   
    //€Õ„‡ è⁄ß   
    //----------------------------------------------------------------------------------------------------   
      //…≤  ‘ÏÅ ƒ„ƒ°œÍÕ„⁄ÂÀ √ı◊ºÊ‡ÿ®≈”⁄ßŒÌ    
      //«≈ÀËÊ≈Å    
      //         true:   ƒ„À √ı◊ºÊ‡√Ÿ…≤ÿ®≈”⁄ßŒÌ   
      //         false:  ƒ„À √ı◊ºÊ‡ﬁÓ«£ÿ®≈”⁄ßŒÌ   
      //¬ﬁ  ⁄ßÅ  *ByteÅ ƒ„À √ı◊ºÊ‡ÿ®≈”⁄ßŒÌ«≈ÀË   
    //-----------------------------------------------------------------------------------------------------   
    /*bool ReadPortRecBuf(uchar Port,Uchar *Byte)  
    {  
    if(CommDevice[Port].CommReceive_Trail==CommDevice[Port].CommReceive_Head)  return false;  
      
    *Byte=CommDevice[Port].CommReceiveBuffer[CommDevice[Port].CommReceive_Trail];  
    CommDevice[Port].CommReceive_Trail=(CommDevice[Port].CommReceive_Trail+1)%CommMaxReceiveBuffer;  
      
    return true;  
    }*/   
    //-----------------------------------------------------------------------------------------------------   
      //…≤  ‘ÏÅ ›öƒ°œÍ«Ä€›À √ı◊ºÊ‡‡ƒ⁄ßŒÌ   
      //«≈ÀËÊ≈Å    
      //         true:   ›öÀ √ı◊ºÊ‡‡ƒ⁄ßŒÌ√Ÿ…≤   
      //         false:  ƒ„À √ı◊ºÊ‡‡ƒ⁄ßŒÌ⁄Ø¿Ï   
      //¬ﬁ  ⁄ßÅ  ByteÅ   ›öÀ √ı◊ºÊ‡‡ƒÿ˚≈‘⁄ßŒÌ   
    //------------------------------------------------------------------------------   
    bool WritePortSendBuf(uchar Port,Uchar Byte)   
    {   
    if((CommDevice[Port].CommSend_Head+1)%CommMaxSendBuffer==CommDevice[Port].CommSend_Trail) return false;   
       
    CommDevice[Port].CommSend_Head=(CommDevice[Port].CommSend_Head+1)%CommMaxSendBuffer;   
    CommDevice[Port].CommSendBuffer[CommDevice[Port].CommSend_Head]=Byte;   
       
    return true;   
    }   
    //--------------------------------------------------------------------------------   
    // ›Ú¬œ–äÂ…ƒ°‡‡œÍ1Ê‡∆ﬂƒ≤–˝√‹‡ê(›Ú¬œÊ‡∆ﬂ0)   
    //--------------------------------------------------------------------------------   
    void serial1_int() interrupt 0   
    {   
        uchar xdata *ptr;   
        uchar event;   
        uchar port;   
           
        port =  CHANL1_PORT;   
        ptr =   CommDevice[port].RAMAddress_Map;                                                // ∆—ISR Ê‡∆ﬂÁƒ‹çÃ‘ƒˆ÷ø   
        event = *(ptr+ISR)&0x0f;                                                                // ∆—Á˛»Ô„’ﬂÿÃ∆¡π≈‘Ê‡∆ﬂ“˚   
        _nop_();   
           
        switch(event)   
              {   
                case EVENT_RXRDY:                                                               // Õ„⁄Ï⁄ßŒÌ≈ÕÊ‡∆ﬂ   
                case EVENT_RXRDY_TIMEOUT:   
                    for(;*(ptr+LSR)&EVENT_LSR_RECEIVE;)   
                        {   
                            CommDevice[port].CommReceiveBuffer[CommDevice[port].CommReceive_Head] = *(ptr+RHR);   
                            if(((CommDevice[port].CommReceive_Head+1)%CommMaxReceiveBuffer)!=CommDevice[port].CommReceive_Trail)   
                                CommDevice[port].CommReceive_Head=(CommDevice[port].CommReceive_Head+1)%CommMaxReceiveBuffer;   
                        }; //end for   
                    break;     
                case EVENT_TXRDY:                                                               //«Ä€›⁄ßŒÌÊ‡∆ﬂ   
                    if(CommDevice[port].CommSend_Trail!=CommDevice[port].CommSend_Head)         //’‡∆ﬂ⁄ßŒÌ¿ù⁄◊«∏«Ä€››˙¡ﬂ   
                        {   
                            for(event=0;(event<FIFO_SIZE)&&(CommDevice[port].CommSend_Trail!=CommDevice[port].CommSend_Head);event++)   
                                {      
                                    CommDevice[port].CommSend_Trail=(CommDevice[port].CommSend_Trail+1)%CommMaxSendBuffer;   
                                    *(ptr+THR) = CommDevice[port].CommSendBuffer[CommDevice[port].CommSend_Trail];   
                                };//end for   
                              CommDevice[port].Comm_Stu = SERIAL_STU_BUSY;   
                        }else CommDevice[port].Comm_Stu = SERIAL_STU_FREE;   
                     break;   
              };//end switch   
    }   
    //--------------------------------------------------------------------------------   
    // ›Ú¬œ–äÂ…ƒ°‡‡œÍ2Ê‡∆ﬂƒ≤–˝√‹‡ê(›Ú¬œÊ‡∆ﬂ2)   
    //--------------------------------------------------------------------------------   
    void serial2_int() interrupt 2   
    {   
        uchar xdata *ptr;   
        uchar event;   
        uchar port;   
           
        port =  CHANL2_PORT;   
        ptr =   CommDevice[port].RAMAddress_Map;                                                //∆—ISR Ê‡∆ﬂÁƒ‹çÃ‘ƒˆ÷ø   
        event = *(ptr+ISR)&0x0f;                                                                //∆—Á˛»Ô„’ﬂÿÃ∆¡π≈‘Ê‡∆ﬂ“˚   
        _nop_();   
           
        switch(event)   
              {   
                case EVENT_RXRDY:                                                               //Õ„⁄Ï⁄ßŒÌ≈ÕÊ‡∆ﬂ   
                case EVENT_RXRDY_TIMEOUT:   
                    for(;*(ptr+LSR)&EVENT_LSR_RECEIVE;)   
                        {   
                            CommDevice[port].CommReceiveBuffer[CommDevice[port].CommReceive_Head] = *(ptr+RHR);   
                            if(((CommDevice[port].CommReceive_Head+1)%CommMaxReceiveBuffer)!=CommDevice[port].CommReceive_Trail)   
                                CommDevice[port].CommReceive_Head=(CommDevice[port].CommReceive_Head+1)%CommMaxReceiveBuffer;   
                        }; //end for   
                    break;     
                case EVENT_TXRDY:                                                               //«Ä€›⁄ßŒÌÊ‡∆ﬂ   
                    if(CommDevice[port].CommSend_Trail!=CommDevice[port].CommSend_Head)         //’‡∆ﬂ⁄ßŒÌ¿ù⁄◊«∏«Ä€››˙¡ﬂ   
                        {   
                            for(event=0;(event<FIFO_SIZE)&&(CommDevice[port].CommSend_Trail!=CommDevice[port].CommSend_Head);event++)   
                                {      
                                    CommDevice[port].CommSend_Trail=(CommDevice[port].CommSend_Trail+1)%CommMaxSendBuffer;   
                                    *(ptr+THR) = CommDevice[port].CommSendBuffer[CommDevice[port].CommSend_Trail];   
                                };//end for   
                            CommDevice[port].Comm_Stu = SERIAL_STU_BUSY;   
                        }else CommDevice[port].Comm_Stu = SERIAL_STU_FREE;   
                     break;   
              };//end switch   
    }   
       
    //--------------------------------------------------------------------------------   
    // ›Ú¬œ–äÂ…ƒ°‡‡œÍ3Ê‡∆ﬂƒ≤–˝√‹‡ê(›Ú¬œÊ‡∆ﬂ6)   
    //--------------------------------------------------------------------------------   
    void serial3_int () interrupt 6   
    {   
        uchar xdata *ptr;   
        uchar event;   
        uchar port;   
           
        port =  CHANL3_PORT;   
        ptr =   CommDevice[port].RAMAddress_Map;                                            // ∆—ISR Ê‡∆ﬂÁƒ‹çÃ‘ƒˆ÷ø   
        event = *(ptr+ISR)&0x0f;                                                            // ∆—Á˛»Ô„’ﬂÿÃ∆¡π≈‘Ê‡∆ﬂ“˚   
        _nop_();   
           
        switch(event)   
              {   
                case EVENT_RXRDY:                                                           // Õ„⁄Ï⁄ßŒÌ≈ÕÊ‡∆ﬂ   
                case EVENT_RXRDY_TIMEOUT:   
                    for(;*(ptr+LSR)&EVENT_LSR_RECEIVE;)   
                        {   
                            CommDevice[port].CommReceiveBuffer[CommDevice[port].CommReceive_Head] = *(ptr+RHR);   
                            if(((CommDevice[port].CommReceive_Head+1)%CommMaxReceiveBuffer)!=CommDevice[port].CommReceive_Trail)   
                                CommDevice[port].CommReceive_Head=(CommDevice[port].CommReceive_Head+1)%CommMaxReceiveBuffer;   
                        }; //end for   
                    break;     
                case EVENT_TXRDY:                                                           //«Ä€›⁄ßŒÌÊ‡∆ﬂ   
                    if(CommDevice[port].CommSend_Trail!=CommDevice[port].CommSend_Head)     //’‡∆ﬂ⁄ßŒÌ¿ù⁄◊«∏«Ä€››˙¡ﬂ   
                        {   
                            for(event=0;(event<FIFO_SIZE)&&(CommDevice[port].CommSend_Trail!=CommDevice[port].CommSend_Head);event++)   
                                {      
                                    CommDevice[port].CommSend_Trail=(CommDevice[port].CommSend_Trail+1)%CommMaxSendBuffer;   
                                    *(ptr+THR) = CommDevice[port].CommSendBuffer[CommDevice[port].CommSend_Trail];   
                                };//end for   
                            CommDevice[port].Comm_Stu = SERIAL_STU_BUSY;   
                        }else CommDevice[port].Comm_Stu = SERIAL_STU_FREE;   
                     break;   
              };//end switch   
    }   
    //--------------------------------------------------------------------------------   
    // ›Ú¬œ–äÂ…ƒ°‡‡œÍ4Ê‡∆ﬂƒ≤–˝√‹‡ê(›Ú¬œÊ‡∆ﬂ7)   
    //--------------------------------------------------------------------------------   
    void serial4_int () interrupt 7   
    {   
        uchar xdata *ptr;   
        uchar event;   
        uchar port;   
           
        port =  CHANL4_PORT;   
        ptr =   CommDevice[port].RAMAddress_Map;                                            // ∆—ISR Ê‡∆ﬂÁƒ‹çÃ‘ƒˆ÷ø   
        event = *(ptr+ISR)&0x0f;                                                            // ∆—Á˛»Ô„’ﬂÿÃ∆¡π≈‘Ê‡∆ﬂ“˚   
        _nop_();   
           
        switch(event)   
              {   
                case EVENT_RXRDY:                                                               // Õ„⁄Ï⁄ßŒÌ≈ÕÊ‡∆ﬂ   
                case EVENT_RXRDY_TIMEOUT:   
                    for(;*(ptr+LSR)&EVENT_LSR_RECEIVE;)   
                        {   
                            CommDevice[port].CommReceiveBuffer[CommDevice[port].CommReceive_Head] = *(ptr+RHR);   
                            if(((CommDevice[port].CommReceive_Head+1)%CommMaxReceiveBuffer)!=CommDevice[port].CommReceive_Trail)   
                                CommDevice[port].CommReceive_Head=(CommDevice[port].CommReceive_Head+1)%CommMaxReceiveBuffer;   
                        }; //end for   
                    break;   
                case EVENT_TXRDY:                                                               //«Ä€›⁄ßŒÌÊ‡∆ﬂ   
                    if(CommDevice[port].CommSend_Trail!=CommDevice[port].CommSend_Head)         //’‡∆ﬂ⁄ßŒÌ¿ù⁄◊«∏«Ä€››˙¡ﬂ   
                        {   
                            for(event=0;(event<FIFO_SIZE)&&(CommDevice[port].CommSend_Trail!=CommDevice[port].CommSend_Head);event++)   
                                {      
                                    CommDevice[port].CommSend_Trail=(CommDevice[port].CommSend_Trail+1)%CommMaxSendBuffer;   
                                    *(ptr+THR) = CommDevice[port].CommSendBuffer[CommDevice[port].CommSend_Trail];   
                                };//end for   
                            CommDevice[port].Comm_Stu = SERIAL_STU_BUSY;   
                        }else CommDevice[port].Comm_Stu = SERIAL_STU_FREE;   
                     break;   
              };//end switch   
    }   
    //--------------------------------------------------------------------------------   