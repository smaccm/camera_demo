package com.rockwellcollins.atc.smaccmviewer;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;
import java.util.zip.DataFormatException;
import java.util.zip.Deflater;
import java.util.zip.Inflater;

class Compressor {
	public Compressor() {
	}

	public byte[] compress(byte[] bytesToCompress) {
		Deflater deflater = new Deflater();
		deflater.setInput(bytesToCompress);
		deflater.finish();

		byte[] bytesCompressed = new byte[Short.MAX_VALUE];

		int numberOfBytesAfterCompression = deflater.deflate(bytesCompressed);

		byte[] returnValues = new byte[numberOfBytesAfterCompression];

		System.arraycopy(bytesCompressed, 0, returnValues, 0,
				numberOfBytesAfterCompression);

		return returnValues;
	}

	public byte[] compress(String stringToCompress) {
		byte[] returnValues = null;

		try {

			returnValues = this.compress(stringToCompress.getBytes("UTF-8"));
		} catch (UnsupportedEncodingException uee) {
			uee.printStackTrace();
		}

		return returnValues;
	}

	public byte[] decompress(byte[] bytesToDecompress, int numberOfBytesToDecompress) {
		byte[] returnValues = null;

		Inflater inflater = new Inflater();

		inflater.setInput(bytesToDecompress, 0, numberOfBytesToDecompress);

		int bufferSizeInBytes = numberOfBytesToDecompress;

		int numberOfBytesDecompressedSoFar = 0;
		List<Byte> bytesDecompressedSoFar = new ArrayList<Byte>();

		try {
			while (inflater.needsInput() == false) {
				byte[] bytesDecompressedBuffer = new byte[bufferSizeInBytes];

				int numberOfBytesDecompressedThisTime = inflater
						.inflate(bytesDecompressedBuffer);

				numberOfBytesDecompressedSoFar += numberOfBytesDecompressedThisTime;

				for (int b = 0; b < numberOfBytesDecompressedThisTime; b++) {
					bytesDecompressedSoFar.add(bytesDecompressedBuffer[b]);
				}
			}

			returnValues = new byte[bytesDecompressedSoFar.size()];
			for (int b = 0; b < returnValues.length; b++) {
				returnValues[b] = (byte) (bytesDecompressedSoFar.get(b));
			}

		} catch (DataFormatException dfe) {
			dfe.printStackTrace();
		}

		inflater.end();

		return returnValues;
	}

	public String decompressToString(byte[] bytesToDecompress) {
		byte[] bytesDecompressed = this.decompress(bytesToDecompress, bytesToDecompress.length);

		String returnValue = null;

		try {
			returnValue = new String(bytesDecompressed, 0,
					bytesDecompressed.length, "UTF-8");
		} catch (UnsupportedEncodingException uee) {
			uee.printStackTrace();
		}

		return returnValue;
	}
}

class Console {
	public static void writeBytesAsHexadecimal(byte[] bytesToWrite) {
		int numberOfBytes = bytesToWrite.length;
		int bytesPerLine = 16;
		int bytesPerColumn = 8;
		int numberOfColumns = bytesPerLine / bytesPerColumn;
		int linesPerBlock = 16;

		int numberOfLines = (int) Math.ceil((double) numberOfBytes
				/ (double) bytesPerLine);

		int widthOfDataOnDisplayInChars = numberOfColumns
				* (bytesPerColumn * 3 + 1) - 1;

		String horizontalRule = "";

		for (int i = 0; i < widthOfDataOnDisplayInChars; i++) {
			horizontalRule += "=";
		}

		System.out.println(horizontalRule);

		for (int y = 0; y < numberOfLines; y++) {
			if (y > 0 && y % linesPerBlock == 0) {
				System.out.println("");
			}

			int bytesOnThisLine;

			if (y < numberOfLines - 1) {
				bytesOnThisLine = bytesPerLine;
			} else {
				bytesOnThisLine = numberOfBytes % bytesPerLine;
			}

			for (int x = 0; x < bytesOnThisLine; x++) {
				if (x > 0 && x % bytesPerColumn == 0) {
					System.out.print("  ");
				}

				int byteIndex = y * bytesPerLine + x;
				byte byteCurrent = bytesToWrite[byteIndex];

				String byteAsString = Integer.toHexString(byteCurrent & 0xFF);
				if (byteAsString.length() < 2) {
					byteAsString = "0" + byteAsString;
				}

				byteAsString += " ";

				System.out.print(byteAsString);
			}

			System.out.println();
		}

		System.out.println(horizontalRule);
	}
}