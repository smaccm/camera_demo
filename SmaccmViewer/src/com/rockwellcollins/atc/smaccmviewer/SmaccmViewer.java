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
		byte[] line = new byte[width*3 + 1];
		DatagramPacket receivePacket = new DatagramPacket(line, line.length); 
		
		while(true){
			
			
			try
			{
				int lineNum = -1;
				for(int p = 0; p < height; p++){
					client.receive(receivePacket);
					lineNum = line[0] & 0xFF;
					for(int i = 1; i < width*3+1; i++){
						pixels[width*lineNum*3 + (i-1)] = line[i];
					}
				}

				//if(capturedWholeFrame){
					BufferedImage image = new BufferedImage(width, height, BufferedImage.TYPE_3BYTE_BGR);
					WritableRaster raster = (WritableRaster) image.getData();
					raster.setPixels(0, 0, width, height, pixels);
					image.setData(raster);
					i = image;
				//}
				
				//on the server side we keep going if anything is received

				//these are acks that are received by the sender to let
				//her know that we are still listening. after it receives
				//a single byte of anything it proceeds to send the next frame
				//client.send(sendPacket);

				
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
