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
		if(devices.length==0){
			System.out.println("No hay interfaces visibles, probablemente necesite permisos apropiados");
			return;
		}
		if(args.length==0){
			for (int i = 0; i < devices.length; i++) {
				  //print out its name and description
				  System.out.println(i+": "+devices[i].name + "(" + devices[i].description+")");

				  //print out its datalink name and description
				  System.out.println(" datalink: "+devices[i].datalink_name + "(" + devices[i].datalink_description+")");

				  //print out its MAC address
				  System.out.print(" MAC address:");
				  for (byte b : devices[i].mac_address)
				    System.out.print(Integer.toHexString(b&0xff) + ":");
				  System.out.println();

				  //print out its IP address, subnet mask and broadcast address
				  for (NetworkInterfaceAddress a : devices[i].addresses)
				    System.out.println(" address:"+a.address + " " + a.subnet + " "+ a.broadcast);
				}
			System.out.println("uso:%java Test n\n n= interfaz a sniffear"); 
			return;
		}
		//for each network interface
		int index=Integer.parseInt(args[0]);
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
