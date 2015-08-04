package com.rockwellcollins.atc.smaccmviewer;

import java.awt.Image;
import java.awt.image.BufferedImage;
import java.awt.image.WritableRaster;
import java.util.Arrays;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.atomic.AtomicReference;
import java.util.zip.DataFormatException;
import java.util.zip.Inflater;

public class ImageProcessor extends Thread {
	private final BlockingQueue<byte[]> frames;
	private final AtomicReference<Image> imageRef;

	public ImageProcessor(BlockingQueue<byte[]> frames, AtomicReference<Image> imageRef) {
		this.frames = frames;
		this.imageRef = imageRef;
	}

	@Override
	public void run() {
		while (true) {
			try {
				byte[] decompressedFrame = decompress(frames.take());

				if (decompressedFrame != null) {
					int[] pixels = new int[decompressedFrame.length];
					for (int i = 0; i < decompressedFrame.length; i += 3) {
						pixels[i] = decompressedFrame[i + 1];
						pixels[i + 1] = decompressedFrame[i + 2];
						pixels[i + 2] = decompressedFrame[i];
					}
					BufferedImage image = new BufferedImage(Constants.WIDTH, Constants.HEIGHT,
							BufferedImage.TYPE_3BYTE_BGR);
					WritableRaster raster = (WritableRaster) image.getData();
					raster.setPixels(0, 0, Constants.WIDTH, Constants.HEIGHT, pixels);
					image.setData(raster);
					imageRef.set(image);
				}
			} catch (InterruptedException | ArrayIndexOutOfBoundsException e) {
				if (Constants.VERBOSE) {
					e.printStackTrace();
				}
			}
		}
	}

	public static byte[] decompress(byte[] compressed) {
		try {
			Inflater inflater = new Inflater();
			inflater.setInput(compressed, 0, compressed.length);
			byte[] result = new byte[Constants.TOTAL_BYTES];
			int len = inflater.inflate(result);
			return Arrays.copyOf(result, len);
		} catch (DataFormatException e) {
			if (Constants.VERBOSE) {
				e.printStackTrace();
			}
			return null;
		}
	}
}
