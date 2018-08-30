// allocate a message of appropriate internal size
NetworkMsg l_sizeMsg;
// get the next byte array
std::vector<char*> l_data = AsioGetNextMsg(); 
// push it into the size message
l_sizeMsg.SetData( l_data.data(), 0 );      
// get the size information from the size message      
int l_size = l_sizeMsg.DeserializeSize();
// initialize the offset to zero   
int l_offset = 0;
// create the actual message container
NetworkMsg l_actualMessage;
// while not enough bytes have been received             
while( l_offset < l_size )
{
  // get the next byte array
  std::vector<char*> l_part = AsioGetNextMsg();   
  // push it in the message container taking into account the offset
  l_actualMessage.SetData( l_part.data(), l_offset);
  // increase the offset
  l_offset += l_part.size();    
}