package com.rockwellcollins.atc.smaccmviewer;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.image.BufferedImage;
import java.awt.image.WritableRaster;
import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.Socket;
import java.net.SocketException;
import java.util.zip.*;

import javax.print.attribute.standard.Compression;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.Timer;


public class SmaccmViewer extends JPanel implements Runnable, ActionListener{

	private static final long serialVersionUID = 8302649087754364851L;
	
//	private String serverName = "192.168.1.123";
//	private final String serverName = "localhost";
	private static String serverName;
	private static int port = 4000;
	
	private Image i = null;
	private static final int width = 320;
	private static final int height = 200;
	private DatagramSocket client;
	private DatagramPacket sendPacket;
	private DatagramPacket receivePacket;
	byte[] sendData = new byte[1];
	private final JFrame frame = new JFrame("SmaccmCopter Video");
	
	private Timer timer = new Timer(50, this);
	
	public static void main(String[] args) {
		
		if(args.length < 1){
			System.out.println("Usage: java SmaccmViewer.jar [hostname|ip address] <port>");
			System.exit(0);
		}
		
		serverName = args[0];
		if(args.length == 2){
			port = Integer.valueOf(args[1]);
		}
		
		final SmaccmViewer viewer = new SmaccmViewer();
		viewer.init();
		Thread thread = new Thread(viewer);
		thread.start();

	}

	public void init() {

		try {
			client = new DatagramSocket();
			InetAddress IPAddress = InetAddress.getByName(serverName);
			sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, port);
			client.send(sendPacket);
			
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(ABORT);
		} 
		
		frame.setSize(width, height);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setVisible(true);
		frame.add(this);
		timer.start();

	}

	public void run() {
		int[] pixels = new int[width*height*3];
		byte[] compressedPixels = new byte[width*height*3];
		//byte[] decompressedPixels = new byte[width*height*3];
		byte[] packet = new byte[width*height*3+2]; //not going to be anywhere near this long
		DatagramPacket receivePacket = new DatagramPacket(packet, packet.length); 
		Compressor compressor = new Compressor();
		
		int receivedPixelIndex = 0;
		int packetIndex = packet[0];
		int totalPackets = packet[1];
		while(true){
			
			try
			{
				client.receive(receivePacket);
				if(packetIndex != 0 && packet[0] != packetIndex+1){
					//we lost a packet
					System.out.println("lost a packet");
					do{
						client.receive(receivePacket);
					}while(packet[0] != 1);
					receivedPixelIndex = 0;
				}
				int receivedLength = receivePacket.getLength();
				packetIndex = packet[0];
				totalPackets = packet[1];
				//rearrange the bytes so we drop the packet statistics
				
				System.out.println("received packet "+packet[0]+" of "+packet[1]);
				for(int i = 2; i < receivedLength; i++){
					compressedPixels[receivedPixelIndex++] = packet[i];
					//System.out.println(compressedPixels[receivedPixelIndex - 1] & 0xFF);
				}

				if(packetIndex == totalPackets){
					
//					Inflater decompresser = new Inflater();
//					decompresser.setInput(decompressedPixels, 0, receivedLength);
//					int resultLength = decompresser.inflate(compressedPixels);
//					decompresser.end();
					byte[] decompressedPixels = compressor.decompress(compressedPixels, receivedPixelIndex);
					if(decompressedPixels != null){
						//stupid conversion to integer pixels
						for(int i = 0; i < decompressedPixels.length; i = i + 3){
							pixels[i] = decompressedPixels[i+1];
							pixels[i+1] = decompressedPixels[i+2];
							pixels[i+2] = decompressedPixels[i];
						}
						BufferedImage image = new BufferedImage(width, height, BufferedImage.TYPE_3BYTE_BGR);
						WritableRaster raster = (WritableRaster) image.getData();
						raster.setPixels(0, 0, width, height, pixels);
						image.setData(raster);
						i = image;
					}
					receivedPixelIndex = 0;
					packetIndex = 0;
				}
//				//}
//				
//				//on the server side we keep going if anything is received
//
//				//these are acks that are received by the sender to let
//				//her know that we are still listening. after it receives
//				//a single byte of anything it proceeds to send the next frame
//				//client.send(sendPacket);
//
//				
			}catch(IOException e)
			{
				e.printStackTrace();
			}
		}
	}

	@Override
	public void actionPerformed(ActionEvent evt) {
		repaint();
	}
	
	@Override
	public void paintComponent(Graphics g){
		if(i != null){
			g.drawImage(i, 0, 0, null);
		}
	}

}
