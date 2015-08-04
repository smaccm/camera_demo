package com.rockwellcollins.atc.smaccmviewer;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.Arrays;

import javax.imageio.ImageIO;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.Timer;

public class SmaccmViewer extends Thread {
	public static final int IMG_WIDTH = 320;
	public static final int IMG_HEIGHT = 200;
	public static final int PACKET_SIZE = 65000;

	public static void main(String[] args) {
		if (args.length < 1) {
			System.out.println("Usage: java SmaccmViewer.jar [hostname|ip address] <port>");
			System.exit(0);
		}

		String serverName = args[0];
		int port = 4000;
		if (args.length == 2) {
			port = Integer.valueOf(args[1]);
		}

		new SmaccmViewer(serverName, port).run();
	}

	public SmaccmViewer(String serverName, int port) {
		this.serverName = serverName;
		this.port = port;
		createUI();
	}

	private final String serverName;
	private final int port;
	private volatile Image image;

	@Override
	public void run() {
		try (DatagramSocket socket = new DatagramSocket()) {
			InetAddress IPAddress = InetAddress.getByName(serverName);
			DatagramPacket helloPacket = new DatagramPacket(new byte[] { 0 }, 1, IPAddress, port);
			socket.send(helloPacket);

			byte[] packet = new byte[PACKET_SIZE];
			DatagramPacket datagramPacket = new DatagramPacket(packet, packet.length);

			int numFrames = 0;
			long start = System.currentTimeMillis();
			while (true) {
				try {
					socket.receive(datagramPacket);
					int len = datagramPacket.getLength();
					byte[] buf = Arrays.copyOf(packet, len);
					image = ImageIO.read(new ByteArrayInputStream(buf));

					numFrames++;
					if (numFrames == 100) {
						long stop = System.currentTimeMillis();
						double seconds = (stop - start) / 1000.0;
						System.out.printf("FPS: %.1f\n", numFrames / seconds);
						numFrames = 0;
						start = stop;
					}
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private void createUI() {
		final JFrame frame = new JFrame();
		frame.setTitle("SmaccmCopter Video");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setVisible(true);
		frame.setSize(IMG_WIDTH, IMG_HEIGHT);

		@SuppressWarnings("serial")
		final JPanel panel = new JPanel() {
			@Override
			public void paintComponent(Graphics g) {
				if (image != null) {
					Dimension size = getSize();
					g.drawImage(image, 0, 0, size.width, size.height, 1, 1, IMG_WIDTH - 2,
							IMG_HEIGHT - 2, null);
				}
			}
		};

		frame.getContentPane().add(panel);

		new Timer(50, new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				panel.repaint();
			}
		}).start();
	}
}
