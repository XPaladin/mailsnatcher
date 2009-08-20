import java.io.IOException;

import jpcap.*;
import jpcap.packet.Packet;




public class Test {

	/**
	 * @param args
	 * @throws IOException 
	 */
	public static void main(String[] args) throws IOException {
		
		//Obtain the list of network interfaces
		NetworkInterface[] devices = JpcapCaptor.getDeviceList();
		//System.out.println(devices.length);
		//for each network interface
		int index=0;
		JpcapCaptor captor=JpcapCaptor.openDevice(devices[index], 65535, false, 20);
		captor.setFilter("ip and tcp", true);
		Packet packet;
		while(true){
			  //capture a single packet
			  packet=captor.getPacket();
			  if(packet!=null){
				  for(int i=0;i<packet.data.length;i++){
					  System.out.print((char)packet.data[i]);
				  }
			  }
		}
	}
}
