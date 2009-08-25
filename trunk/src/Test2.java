import java.awt.Image;
import java.util.ArrayList;
import java.util.List;

import org.jnetpcap.Pcap;
import org.jnetpcap.PcapIf;
import org.jnetpcap.packet.JPacket;
import org.jnetpcap.packet.JRegistry;
import org.jnetpcap.packet.analysis.JController;
import org.jnetpcap.protocol.network.Ip4;
import org.jnetpcap.protocol.tcpip.Http;
import org.jnetpcap.protocol.tcpip.HttpAnalyzer;
import org.jnetpcap.protocol.tcpip.HttpHandler;
import org.jnetpcap.protocol.tcpip.Tcp;
import org.jnetpcap.protocol.tcpip.Http.ContentType;
import org.jnetpcap.protocol.tcpip.Http.Request;
import org.jnetpcap.protocol.tcpip.Http.Response;

/**
 * @author Mark Bednarczyk
 * @author Sly Technologies, Inc.
 */
public class Test2 {

	public static void main(String[] args) {

		/*
		 * JController implements JControllerOptions but we downcast to
		 * JControllerOptions since we are only interested in changing its
		 * options. By downcasting we narrow down what the IDEs will suggest, to
		 * only option accessor methods.
		 */
		// JControllerOptions controller =
		// JRegistry.getAnalyzer(JController.class);

		/**
		 * No packets will be dispatched to JPacketHandler, since there isn't
		 * one anyway. All packets that have been analyzed will be consumed.
		 */
		// controller.consumePackets(true);

		/**
		 * Step 1 - add our Http handler to HttpAnalyzer. Get HttpAnalyzer from
		 * registry, it should already be registered.
		 */
		HttpAnalyzer httpAnalyzer = JRegistry.getAnalyzer(HttpAnalyzer.class);
		HttpHandler handler = new HttpHandler() {

			/*
			 * Step 2 - our handler routine.
			 */
			public void processHttp(Http http) {

				JPacket packet = http.getPacket(); // Packet this http belongs
													// to
				final long frame = packet.getFrameNumber();

				System.out.printf("\n#%-3d: %8s", frame, http.getMessageType());

				if (http.getMessageType() == null) {
					System.out.printf(" http=%s\ntcp=%s\nip=%s", http, packet
							.getHeader(new Tcp()), packet.getHeader(new Ip4()));
				}

				if (http.getMessageType() == Http.MessageType.REQUEST) {
					System.out.printf(" url:%s", http
							.fieldValue(Request.RequestUrl));
				}

				// if (http.getPayloadLength() > 0) {
				// System.out.printf("\n#%-3d: http=%s", frame, http);
				// }

				/*
				 * Http is normal http header that has been decoded.
				 */
				if (http.getMessageType() != Http.MessageType.RESPONSE) {
					return;
				}

				/*
				 * Responses always have a content type, since we are looking
				 * for specific content that has been predefined, we can use
				 * enum constants. We're not interested in anything else,
				 * otherwise we'd have to use http.contentType() method which
				 * returns a string.
				 */
				ContentType type = http.contentTypeEnum();

				switch (type) {

				default:
					if (http.contentType() == null) {
						System.out.printf(" Found content type %s", http
								.contentType());
					} else {
						System.out.printf(" method %s", http
								.fieldValue(Response.ResponseCode));

					}
				}
			}

		};

		httpAnalyzer.add(handler);

		/*
		 * Step 3 - normal open capture file stuff
		 */
		StringBuilder errbuf = new StringBuilder();

		//Pcap pcap = Pcap.openOffline("/home/psepulv/asdad", errbuf);
		List<PcapIf> alldevs = new ArrayList<PcapIf>(); // Will be filled with
														// NICs
	//	StringBuilder errbuf = new StringBuilder(); // For any error msgs

		/********************************************
		 * First get a list of devices on this system
		 ********************************************/
		int r = Pcap.findAllDevs(alldevs, errbuf);
		if (r == Pcap.NOT_OK || alldevs.isEmpty()) {
			System.err.printf("Can't read list of devices, error is %s", errbuf
					.toString());
			return;
		}

		System.out.println("Network devices found:");

		int i = 0;
		for (PcapIf device : alldevs) {
			System.out.printf("#%d: %s [%s]\n", i++, device.getName(), device
					.getDescription());
		}

		PcapIf device = alldevs.get(2); // We know we have atleast 1 device
		System.out.printf("\nChoosing '%s' on your behalf:\n", device
				.getDescription());

		/***************************************
		 * Second we open up the selected device
		 ***************************************/
		int snaplen = 64 * 1024; // Capture all packets, no truncation
		int flags = Pcap.MODE_PROMISCUOUS; // capture all packets
		int timeout = 10 * 1000; // 10 seconds in millis
		Pcap pcap = Pcap.openLive(device.getName(), snaplen, flags, timeout,
				errbuf);
		if (pcap == null) {
			System.err.println(errbuf.toString());
			System.exit(1);
		}

		/*
		 * Step 4 - We enter our loop. The main thing to consider is that we are
		 * passing JController as the primary handler for libpcap packets.
		 * Notice JController is registered with JRegistry. It knows how to pass
		 * packets on to other analyzers, especially the HttpAnalyzer in our
		 * case. We could add a JPacketHandler as a listener to JController,
		 * which would pass on regular packets back to us, after they have been
		 * analyzed. This is the same, as if we registered our handler directly
		 * with the loop. All packets will be passed back to us. There are
		 * however few differenced. The main difference is that packet returned
		 * from JController as opposed to directly from the Pcap.loop or
		 * dispatch methods, is that JController may have buffered them, while
		 * analyzers where working through the stream of packets. Once the
		 * appropriate release signals were sent by any analyzer that had a hold
		 * on JController's outbound queue, all the packets are fetched to
		 * JPacketHandler listeners as the outboud queue is drained. In our
		 * example we chose to work with HttpAnalyzer directly and its
		 * HttpHandler, which is more specific to http protocol.
		 */
		pcap.loop(Pcap.LOOP_INFINATE, JRegistry.getAnalyzer(JController.class),
				null);

		// pcap.loop(Pcap.LOOP_INFINATE, new JPacketHandler<Object>() {
		// private Http http = new Http();
		//
		// public void nextPacket(JPacket packet, Object user) {
		// if (packet.hasHeader(http)
		// && http.getMessageType() == Http.MessageType.RESPONSE) {
		// System.out.printf("#%d http=%s\n", packet.getFrameNumber(), http
		// , http.toHexdump());
		// }
		// }
		//
		// }, null);

		/*
		 * Always close the pcap handle after we are done
		 */
		pcap.close();
		System.exit(0);
	}

}