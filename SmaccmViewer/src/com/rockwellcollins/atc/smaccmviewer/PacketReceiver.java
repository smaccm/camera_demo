package com.rockwellcollins.atc.smaccmviewer;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.concurrent.BlockingQueue;

public class PacketReceiver extends Thread {
	private final BlockingQueue<byte[]> frames;
	private final DatagramSocket socket;

	public PacketReceiver(String serverName, int port, BlockingQueue<byte[]> frames)
			throws IOException {
		this.frames = frames;

		socket = new DatagramSocket();
		InetAddress IPAddress = InetAddress.getByName(serverName);
		DatagramPacket helloPacket = new DatagramPacket(new byte[] { 0 }, 1, IPAddress, port);
		socket.send(helloPacket);
	}

	@Override
	public void run() {
		byte[] frame = new byte[Constants.TOTAL_BYTES];
		byte[] packet = new byte[Constants.TOTAL_BYTES + 2];
		DatagramPacket datagramPacket = new DatagramPacket(packet, packet.length);

		boolean reset = true;
		int expectedPacket = 0;
		int totalPackets = 0;
		int frameLength = 0;

		while (true) {
			try {
				socket.receive(datagramPacket);
			} catch (IOException e) {
				e.printStackTrace();
				continue;
			}
			
			if (reset) {
				if (packet[0] != 1) {
					System.out.println("Missed a packet, expected 1 but got " + packet[0]);
					continue;
				}
				expectedPacket = 1;
				totalPackets = packet[1];
				frameLength = 0;
				reset = false;
			} else if (packet[0] != expectedPacket ) {
				System.out.println("Missed a packet, expected " + expectedPacket + " but got " + packet[0]);
				reset = true;
				continue;
			}
			
			int length = datagramPacket.getLength() - 2;
			System.arraycopy(packet, 2, frame, frameLength, length);
			frameLength += length;
			if (expectedPacket == totalPackets) {
				byte[] copy = new byte[frameLength];
				System.arraycopy(frame, 0, copy, 0, frameLength);
				frames.add(copy);
				reset = true;
			} else {
				expectedPacket++;
			}
		}
	}
}
