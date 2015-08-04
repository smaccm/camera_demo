package com.rockwellcollins.atc.smaccmviewer;

import java.awt.image.BufferedImage;
import java.awt.image.WritableRaster;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.concurrent.ConcurrentLinkedQueue;

public class Receiver extends Thread {
	private final ConcurrentLinkedQueue<byte[]> frames;
	private final DatagramSocket socket;

	public Receiver(String serverName, int port, ConcurrentLinkedQueue<byte[]> frames)
			throws IOException {
		this.frames = frames;

		socket = new DatagramSocket();
		InetAddress IPAddress = InetAddress.getByName(serverName);
		DatagramPacket helloPacket = new DatagramPacket(new byte[] { 0 }, 1, IPAddress, port);
		socket.send(helloPacket);
	}

	@Override
	public void run() {
		int[] frame = new int[SmaccmViewer.WIDTH * SmaccmViewer.HEIGHT * 3];
		byte[] packet = new byte[frame.length + 2];
		DatagramPacket datagramPacket = new DatagramPacket(packet, packet.length);

		boolean reset = true;
		int currPacket = 0;
		int totalPackets = 0;
		int frameLength = 0;

		int todo_how_to_kill_thread;
		while (true) {
			try {
				socket.receive(datagramPacket);
			} catch (IOException e) {
				e.printStackTrace();
				continue;
			}
			
			if (reset) {
				if (packet[0] != 1) {
					System.out.println("Missed a header packet");
					continue;
				}
				currPacket = 1;
				totalPackets = packet[1];
				frameLength = 0;
				reset = false;
			} else if (packet[0] != currPacket + 1) {
				System.out.println("Missed a body packet");
				reset = true;
				continue;
			}
			
			int length = datagramPacket.getLength();
			System.arraycopy(packet, 0, frame, frameLength, length);
			frameLength += length;
			if (currPacket == totalPackets) {
				byte[] copy = new byte[frameLength];
				System.arraycopy(frame, 0, copy, 0, frameLength);
				frames.add(copy);
				reset = true;
			} else {
				currPacket++;
			}
		}
	}
}
