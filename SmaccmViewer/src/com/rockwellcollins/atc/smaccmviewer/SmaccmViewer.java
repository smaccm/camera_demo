package com.rockwellcollins.atc.smaccmviewer;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Image;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.net.BindException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.Arrays;

import javax.imageio.ImageIO;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;

public class SmaccmViewer extends Thread {
	public static final int IMG_WIDTH = 320;
	public static final int IMG_HEIGHT = 200;
	public static final int PACKET_SIZE = 65000;

	public static void main(String[] args) {
		int port = 4000;
		if (args.length == 1) {
			port = Integer.valueOf(args[0]);
		}

		new SmaccmViewer(port).run();
	}

	public SmaccmViewer(int port) {
		this.port = port;
		createUI();
	}

	private final int port;
	private volatile Image image;
	private JFrame frame;
	private JPanel panel;

	@Override
	public void run() {
		try (DatagramSocket socket = new DatagramSocket(port)) {
			byte[] packet = new byte[PACKET_SIZE];
			DatagramPacket datagramPacket = new DatagramPacket(packet, packet.length);

			while (true) {
				try {
					socket.receive(datagramPacket);
					int len = datagramPacket.getLength();
					byte[] buf = Arrays.copyOf(packet, len);
					image = ImageIO.read(new ByteArrayInputStream(buf));
					refreshDisplay(datagramPacket.getAddress());
					displayFps();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		} catch (BindException e) {
			e.printStackTrace();
			System.exit(-1);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private void refreshDisplay(final InetAddress inetAddress) {
		SwingUtilities.invokeLater(new Runnable() {
			@Override
			public void run() {
				frame.setTitle("SMACCMcopter: " + inetAddress.getHostAddress());
				panel.repaint();
			}
		});
	}

	int numFrames = 0;
	long start = System.currentTimeMillis();

	private void displayFps() {
		numFrames++;
		long stop = System.currentTimeMillis();
		double seconds = (stop - start) / 1000.0;
		if (seconds > 1) {
			System.out.printf("FPS: %.1f\n", numFrames / seconds);
			numFrames = 0;
			start = stop;
		}
	}

	@SuppressWarnings("serial")
	private void createUI() {
		frame = new JFrame();
		frame.setTitle("SMACCMcopter");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setVisible(true);
		frame.setSize(IMG_WIDTH, IMG_HEIGHT);

		panel = new JPanel() {
			@Override
			public void paintComponent(Graphics g) {
				if (image != null) {
					Dimension size = getSize();
					g.drawImage(image, 0, 0, size.width, size.height, 0, 0, IMG_WIDTH - 2,
							IMG_HEIGHT - 2, null);
				}
			}
		};
		frame.getContentPane().add(panel);
	}
}