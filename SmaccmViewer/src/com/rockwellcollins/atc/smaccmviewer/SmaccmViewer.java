package com.rockwellcollins.atc.smaccmviewer;

import java.awt.Graphics;
import java.awt.Image;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.atomic.AtomicReference;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.Timer;

public class SmaccmViewer {

	public static void main(String[] args) throws IOException {
		if (args.length < 1) {
			System.out.println("Usage: java SmaccmViewer.jar [hostname|ip address] <port>");
			System.exit(0);
		}

		String serverName = args[0];
		int port = 4000;
		if (args.length == 2) {
			port = Integer.valueOf(args[1]);
		}

		BlockingQueue<byte[]> frames = new LinkedBlockingQueue<>();
		AtomicReference<Image> imageRef = new AtomicReference<>();

		new PacketReceiver(serverName, port, frames).start();
		new ImageProcessor(frames, imageRef).start();
		createUI(imageRef);
	}

	private static void createUI(final AtomicReference<Image> imageRef) {
		final JFrame frame = new JFrame();
		frame.setTitle("SmaccmCopter Video");
		frame.setSize(Constants.WIDTH, Constants.HEIGHT);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setVisible(true);

		@SuppressWarnings("serial")
		final JPanel panel = new JPanel() {
			@Override
			public void paintComponent(Graphics g) {
				Image image = imageRef.get();
				if (image != null) {
					g.drawImage(image, 0, 0, null);
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
