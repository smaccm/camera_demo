package com.rockwellcollins.atc.smaccmviewer;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.GridBagLayout;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.net.BindException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.Arrays;

import javax.imageio.ImageIO;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;
import javax.swing.Timer;

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
	private volatile BufferedImage image;
	private volatile boolean dimmed = false;

	private JFrame frame;
	private JPanel panel;
	private JLabel label;

	private double lastFrameMs;

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
					lastFrameMs = System.currentTimeMillis();
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
		Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
		int width = screenSize.width / 2;
		int height = width * IMG_HEIGHT / IMG_WIDTH;

		frame = new JFrame();
		frame.setTitle("SMACCMcopter");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setVisible(true);
		frame.setSize(width, height);

		panel = new JPanel() {
			@Override
			public void paintComponent(Graphics g) {
				Dimension size = getSize();

				double delay = (System.currentTimeMillis() - lastFrameMs) / 1000.0;
				if (image != null && delay > 2.0) {
					label.setVisible(true);
					label.setText(String.format(" Connection lost: %.1fs ", delay));
					double newSize = Math.min(size.getHeight() / 10, size.getWidth() / 20);
					label.setFont(label.getFont().deriveFont((float) newSize));

					if (!dimmed) {
						dimImage();
						dimmed = true;
					}
				} else {
					dimmed = false;
					label.setVisible(false);
				}

				if (image != null) {
					g.drawImage(image, 0, 0, size.width, size.height, 0, 0, IMG_WIDTH - 2,
							IMG_HEIGHT - 2, null);
				}
			}
		};

		panel.setLayout(new GridBagLayout());
		label = new JLabel();
		label.setHorizontalAlignment(SwingConstants.CENTER);
		label.setOpaque(true);
		label.setForeground(Color.RED);
		label.setBackground(Color.BLACK);
		label.setVisible(false);
		panel.add(label);

		frame.setMinimumSize(new Dimension(IMG_WIDTH, IMG_HEIGHT));

		// Auto-refresh for when connection is lost
		new Timer(100, new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				panel.repaint();
			}
		}).start();

		frame.getContentPane().add(panel);
	}

	private void dimImage() {
		for (int i = 0; i < image.getWidth(); i++) {
			for (int j = 0; j < image.getHeight(); j++) {
				Color c = new Color(image.getRGB(i, j));
				int average = (c.getRed() + c.getGreen() + c.getBlue()) / 3;
				Color newColor = new Color(average, average, average);
				image.setRGB(i, j, newColor.getRGB());
			}
		}
	}
}
