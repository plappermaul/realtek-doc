/*
 * Copyright (C) 2011 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * Purpose : I/O read/write APIs using RSP to access target memory through RLX-probe in the SDK.
 *
 * Feature : I/O read/write APIs, by RSP
 *
 */

/*
 * Include Files
 */
#include <common/error.h>
#include <common/util.h>
#include <ioal/ioal_init.h>
#include <ioal/io_rsp.h>

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */

int rsp_sockfd;


/*
 * Macro Declaration
 */
#define RLX_DEBUG  0 


void decode_data (char *data_p,unsigned int *value);
void io_rsp_init(void);
void io_rsp_shutdown(void);
void io_rsp_connect(void);
int
remote_escape_output (char *buffer, int len,
		      char *out_buf, int *out_len,
		      int out_maxlen);
static int
remote_unescape_input (char *buffer, int len,
		       char *out_buf, int out_maxlen);


/* Look for a sequence of characters which can be run-length encoded.
   If there are any, update *CSUM and *P.  Otherwise, output the
   single character.  Return the number of characters consumed.  */
static int
try_rle (char *buf, int remaining, unsigned char *csum, char **p)
{

#if RLX_DEBUG
  printf("\n%s",__FUNCTION__);
#endif

  /* Always output the character.  */
  *csum += buf[0];
  *(*p)++ = buf[0];
    

#if 0
  int n;

  /* Don't go past '~'.  */
  if (remaining > 97)
    remaining = 97;

  for (n = 1; n < remaining; n++)
    if (buf[n] != buf[0])
      break;

  /* N is the index of the first character not the same as buf[0].
     buf[0] is counted twice, so by decrementing N, we get the number
     of characters the RLE sequence will replace.  */
  n--;

  if (n < 3)
  {
  return 1;  
  }
  /* Skip the frame characters.  The manual says to skip '+' and '-'
     also, but there's no reason to.  Unfortunately these two unusable
     characters double the encoded length of a four byte zero
     value.  */
  while (n + 29 == '$' || n + 29 == '#')
  {
    n--;
  }



  *csum += '*';
  *(*p)++ = '*';
  *csum += n + 29;
  *(*p)++ = n + 29;


  return n + 1;
#else
    if(remaining);
    
    return 1;
#endif
}
/* Convert number NIB to a hex digit.  */

static int
tohex (int nib)
{
  if (nib < 10)
    return '0' + nib;
  else
    return 'a' + nib - 10;
}




/* Convert BUFFER, escaped data LEN bytes long, into binary data
   in OUT_BUF.  Return the number of bytes written to OUT_BUF.
   Raise an error if the total number of bytes exceeds OUT_MAXLEN.

   This function reverses remote_escape_output.  It allows more
   escaped characters than that function does, in particular because
   '*' must be escaped to avoid the run-length encoding processing
   in reading packets.  */

static int
remote_unescape_input (char *buffer, int len,
		       char *out_buf, int out_maxlen)
{
  int input_index, output_index;
  int escaped;

  output_index = 0;
  escaped = 0;
  for (input_index = 0; input_index < len; input_index++)
    {
        char b = buffer[input_index];
    
        if (output_index + 1 > out_maxlen)
    	    printf ("Received too much data from the target.");
    
        if (escaped)
    	{
    	    out_buf[output_index++] = b ^ 0x20;
    	    escaped = 0;
    	}
        else if (b == '}')
        {
    	    escaped = 1;
        }
        else
    	    out_buf[output_index++] = b;
    }

  if (escaped)
    printf ("Unmatched escape character in target response.");

  return output_index;
}


/* Convert BUFFER, binary data at least LEN bytes long, into escaped
   binary data in OUT_BUF.  Set *OUT_LEN to the length of the data
   encoded in OUT_BUF, and return the number of bytes in OUT_BUF
   (which may be more than *OUT_LEN due to escape characters).  The
   total number of bytes in the output buffer will be at most
   OUT_MAXLEN.  */

int
remote_escape_output (char *buffer, int len,
		      char *out_buf, int *out_len,
		      int out_maxlen)
{
  int input_index, output_index;
  int out_cnt=0;
  output_index = 0;
  for (input_index = 0; input_index < len; input_index++)
  {
      char b = buffer[input_index];

      //if (b == '$' || b == '#' || b == '}' || b == '*')
      if (b == '$' || b == '#' || b == '}')
	  {
    	  /* These must be escaped.  */
    	  if (output_index + 2 > out_maxlen)
    	    break;
    	  out_buf[output_index++] = '}';
    	  out_cnt++;
    	  out_buf[output_index++] = b ^ 0x20;
    	  out_cnt++;

	  }
      else
	  {
	    if (output_index + 1 > out_maxlen)
	        break;
	    out_buf[output_index++] = b;
   	    out_cnt++;

	  }
  }

  *out_len = out_cnt;
  return out_cnt;
}


static int
putpkt_to_rsp (char *buf, int cnt)
{
  int i;
  unsigned char csum = 0;
  char buf2[1024];
  char *p,*ptmp;
  int escpLen;
  /* Copy the packet into buffer BUF2, encapsulating it
     and giving it a checksum.  */
  
  p = buf2;
  *p++ = '$';

  ptmp = p;  

    
  remote_escape_output (buf, cnt,
		      ptmp, &escpLen,
		      sizeof(buf2)-1);

#if RLX_DEBUG    
  printf("\ninput len:%d  escpLen :%d\n",cnt,escpLen);
    
  printf("\n%s:%s  :len:%d\n",__FUNCTION__,buf2,escpLen+1);
  
  for (i = 0; i < escpLen+1; i++)
  {
      printf("[%d]:%2.2x   :%c\n",i,buf2[i],buf2[i]);    
  
  }  
#endif 
    

  for (i = 0; i < escpLen;)
    i += try_rle (ptmp + i, escpLen - i, &csum, &p);


  *p++ = '#';
  *p++ = tohex ((csum >> 4) & 0xf);
  *p++ = tohex (csum & 0xf);

  *p = '\0';
    
#if RLX_DEBUG  
  printf("\n%s:%s  :len:%d\n",__FUNCTION__,buf2,(p - buf2));
  
  for (i = 0; i < (p - buf2); i++)
  {
      printf("[%d]:%2.2x   :%c\n",i,buf2[i],buf2[i]);    
  
  }  
#endif
  
  /* Send it over socket.  */
  if(send(rsp_sockfd, buf2, p - buf2,0)<0)
  {
      printf("\n\nerror send pkt to RLX!!\n");  
      return -1;

  }   

  return 1;
  
}

/*Read from RSP the format is

 +$<data>#<2 byte check sum>
 this function will return data part to buffer 
*/
static int
getpkt_from_rsp (char *buffer)
{
  char rx_char;
  char *p;
  int n;
  int is_first=1,is_end=0,is_error=0;
  int remain_byte;
  
  p = buffer;
  
  while(1)
  {       
      if(recv(rsp_sockfd,&rx_char,1,0)<=0)
      {
          printf("\nsocket rx failure(maybe access invalid memory address!)!!\n");
          
          /*re-connect socket*/
          io_rsp_init();
          return -1;
      }
#if RLX_DEBUG       
      printf("\nrx:0x%x    %c\n",rx_char,rx_char);
#endif      
      if(is_end == 1)
      {
        remain_byte --;
        if(remain_byte == 0)
            break;
        continue;  
      }
      if(rx_char == '+' && is_first ==1)
        continue;
      if(rx_char == '$' && is_first ==1)
      {
        is_first =1;
        continue;
      }  
      /*error occurs */  
      if(rx_char == 'E')
      {
        is_error = 1;  
        continue;    
      }
            
      if(rx_char == '#')
      {
        is_end = 1;  
        remain_byte = 2;
        continue;    
      }
      *p =  rx_char;
      p++;
        

  }  
  *p = 0;    
  
  n = p - buffer;
  
  
  if(is_error == 0)  
      return n;
  else
      return -1;
}



/*set packet format

  X<address>,4:<data>
*/
int io_rsp_memory_write(uint32 memaddr,uint32 data)
{
    char buf[64];
    char rx_buf[64];
    char *p,*p_data;
    int len,i;
    uint32 tmp_data;

    io_rsp_connect();

    if(rsp_sockfd == 0)
        return 0;


    memset(buf, 0x0,sizeof(buf));
    memset(rx_buf, 0x0,sizeof(rx_buf));
    p = buf;
    
    /*prepare X*/
    *p = 'X';
    p++;
    
    /*prepare <address>,4:*/
    sprintf(p,"%8.8x,4:",memaddr);
    

    p = p + strlen(p);
    
    tmp_data = SWAP_L32(data);
    /*copy <data> to buf*/
    p_data = (char *)&tmp_data;
    for(i=0;i<4;i++)
    {
        *p = *p_data;
        p++;
        p_data++;
    }
   
    len = p - buf;
   
    /*call RLX_TX API*/
    putpkt_to_rsp(buf,len);
    
    /*call RLX RX API to get result*/    
    //printf("%s %d",__FUNCTION__,__LINE__);
    if(getpkt_from_rsp(rx_buf)== -1)
    {
        io_rsp_shutdown();

        return -1;
    }    

    io_rsp_shutdown();
    
    return 1;    
}

/* Convert hex digit A to a number.  */

static int
fromhex (int a)
{
  if (a >= '0' && a <= '9')
    return a - '0';
  else if (a >= 'a' && a <= 'f')
    return a - 'a' + 10;
  else
    printf ("Reply contains invalid hex digit");
  return 0;
}


void
decode_data (char *data_p,unsigned int *value)
{
  char ch;
  int i;
  uint32 t_data;  
  t_data = 0;
  for (i = 0; i < 8; i++)
    {
      ch = data_p[i];
      t_data = t_data << 4;
      t_data = t_data | (fromhex (ch) & 0x0f);
    }
  *value = t_data;
}


/*set packet format
  m<address>,4:
*/
uint32 io_rsp_memory_read(uint32 memaddr)
{
    char buf[64];
    char rx_buf[64];
    char *p;
    int len;
    uint32 value; 
    
    io_rsp_connect();
    
    if(rsp_sockfd == 0)
        return 0;
    
    memset(buf, 0x0,sizeof(buf));
    memset(rx_buf, 0x0,sizeof(rx_buf));
    
    p = buf;
    
    /*prepare m*/
    *p = 'm';
    p++;    

    /*prepare <address>,4:*/
    sprintf(p,"%8.8x,4:",memaddr);

    p = p + strlen(p);
    len = p - buf;
    
    /* call RLX_TX API */
    putpkt_to_rsp(buf,len);
    
    /*call RLX RX API to get result */
    len = getpkt_from_rsp(rx_buf);
    if(len < 0)
    {
        io_rsp_shutdown();
        return 0;
    }
    else
    {
        remote_unescape_input(rx_buf, len,
		       buf, sizeof(buf));
        
        
        decode_data(buf,&value);
        //printf("\nget value:0x%x\n",value);
        io_rsp_shutdown();

        return value;
    }
}


void io_rsp_init(void)
{
#if 1
    /* JTAG I/O init */    
	struct sockaddr_in dest;

    /* create socket */
    rsp_sockfd = socket(PF_INET,SOCK_STREAM,0);
    
   
    if(rsp_sockfd <= 0)
    {
        printf("\nError can not create socket!!\n");
        return;
    }
	/* initialize value in dest */
	#if 0
	bzero(&dest, sizeof(dest));
	#else
	osal_memset(&dest,0x0, sizeof(dest));
	#endif
	
	
	dest.sin_family = PF_INET;
	dest.sin_port = htons(5181);
    if(inet_aton("127.0.0.1", &dest.sin_addr) == 0)
    {
        printf("\n%s invalid IP address!!\n", __FILE__);
        return;
    }

	/* Connecting to server */
	if(connect(rsp_sockfd, (struct sockaddr*)&dest, sizeof(dest))<0)
	{
	    printf("\nconnect failure~~\n");    
        close(rsp_sockfd);
    	rsp_sockfd = 0;    

	}
#endif
}    

void io_rsp_connect(void)
{
#if 0
    /* JTAG I/O init */    
	struct sockaddr_in dest;

    /* create socket */
    rsp_sockfd = socket(PF_INET,SOCK_STREAM,0);
    
   
    if(rsp_sockfd == 0)
        printf("\nError can not create socket!!\n");
    
	/* initialize value in dest */
	#if 0
	bzero(&dest, sizeof(dest));
	#else
	osal_memset(&dest,0x0, sizeof(dest));
	#endif
	
	
	dest.sin_family = PF_INET;
	dest.sin_port = htons(5181);
	inet_aton("127.0.0.1", &dest.sin_addr);

	/* Connecting to server */
	if(connect(rsp_sockfd, (struct sockaddr*)&dest, sizeof(dest))<0)
	{
	    printf("\nconnect failure~~\n");    
    	rsp_sockfd = 0;    

	}
#endif	
}    

void io_rsp_shutdown(void)
{
#if 0
    shutdown(rsp_sockfd,2);    
#endif
}
