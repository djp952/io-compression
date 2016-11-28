//---------------------------------------------------------------------------
// Copyright (c) 2016 Michael G. Brehm
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------

using System;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Reflection;
using System.Text;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace zuki.io.compression.test
{
	[TestClass()]
	public class TestGzip
	{
		static byte[] s_sampledata;

		[ClassInitialize()]
		public static void ClassInit(TestContext context)
		{
			// Load the sample data into a byte[] array to use for the unit tests
			using (StreamReader reader = new StreamReader(Assembly.GetExecutingAssembly().GetManifestResourceStream("zuki.io.compression.test.thethreemusketeers.txt")))
			{
				s_sampledata = Encoding.ASCII.GetBytes(reader.ReadToEnd());
			}
		}

		[TestMethod(), TestCategory("Gzip")]
		public void Gzip_CompressExternal()
		{
			// This method generates an output file that can be tested externally; "thethreemusketeers.txt" is
			// set to Copy Always to the output directory, it can be diffed after running the external tool
			using (GzipWriter writer = new GzipWriter(File.Create(Path.Combine(Environment.CurrentDirectory, "thethreemusketeers.gz"))))
			{
				writer.Write(s_sampledata);
				writer.Flush();
			}
		}

		[TestMethod(), TestCategory("Gzip")]
		public void Gzip_DecompressExternal()
		{
			// Decompress a stream created externally to this library
			using (GzipReader reader = new GzipReader(Assembly.GetExecutingAssembly().GetManifestResourceStream("zuki.io.compression.test.thethreemusketeers.gz")))
			{
				using (MemoryStream dest = new MemoryStream())
				{
					reader.CopyTo(dest);
					dest.Flush();

					// Verify that the output matches the sample data byte-for-byte
					Assert.IsTrue(Enumerable.SequenceEqual(s_sampledata, dest.ToArray()));
				}
			}
		}

		[TestMethod(), TestCategory("Gzip")]
		public void Gzip_CompressDecompress()
		{
			// Start with a MemoryStream created from the sample data
			using (MemoryStream source = new MemoryStream(s_sampledata))
			{
				using (MemoryStream dest = new MemoryStream())
				{
					// Compress the data into the destination memory stream instance
					using (GzipWriter compressor = new GzipWriter(dest, CompressionLevel.Optimal, true)) source.CopyTo(compressor);

					// The compressed data should be smaller than the source data
					Assert.IsTrue(dest.Length < source.Length);

					source.SetLength(0);            // Clear the source stream
					dest.Position = 0;              // Reset the destination stream

					// Decompress the data back into the source memory stream
					using (GzipReader decompressor = new GzipReader(dest, true)) decompressor.CopyTo(source);

					// Ensure that the original data has been restored
					Assert.AreEqual(source.Length, s_sampledata.Length);
					Assert.IsTrue(s_sampledata.SequenceEqual(source.ToArray()));
				}
			}
		}

		[TestMethod(), TestCategory("Gzip")]
		public void Gzip_CompressionLevel()
		{
			using (MemoryStream source = new MemoryStream(s_sampledata))
			{
				long none, fastest, optimal;        // Size of compressed streams

				// Compress using CompressionLevel.NoCompression
				using (MemoryStream compressed = new MemoryStream())
				{
					using (GzipWriter compressor = new GzipWriter(compressed, CompressionLevel.NoCompression))
					{
						compressor.Write(s_sampledata, 0, s_sampledata.Length);
						compressor.Flush();
						none = compressed.Length;
					}
				}

				// Compress using CompressionLevel.Fastest
				using (MemoryStream compressed = new MemoryStream())
				{
					using (GzipWriter compressor = new GzipWriter(compressed, CompressionLevel.Fastest))
					{
						compressor.Write(s_sampledata, 0, s_sampledata.Length);
						compressor.Flush();
						fastest = compressed.Length;
					}
				}

				// Compress using CompressionLevel.Optimal
				using (MemoryStream compressed = new MemoryStream())
				{
					using (GzipWriter compressor = new GzipWriter(compressed, CompressionLevel.Optimal))
					{
						compressor.Write(s_sampledata, 0, s_sampledata.Length);
						compressor.Flush();
						optimal = compressed.Length;
					}
				}

				// Fastest should produce better results than no compression
				Assert.IsTrue(fastest < none);

				// Optimal should produce better results than fastest
				Assert.IsTrue(optimal < fastest);

				// Fastest should be smaller than the original length
				Assert.IsTrue(fastest < source.Length);

				// No compression should be BIGGER than the original length
				Assert.IsTrue(source.Length < none);
			}
		}

		[TestMethod(), TestCategory("Gzip")]
		public void Gzip_Dispose()
		{
			byte[] buffer = new byte[8192];         // 8KiB data buffer

			// Create a dummy stream and immediately dispose of it
			GzipWriter stream = new GzipWriter(new MemoryStream(s_sampledata), CompressionLevel.Optimal);
			stream.Dispose();

			// All properties and methods should throw an ObjectDisposedException
			try { var bs = stream.BaseStream; Assert.Fail("Property access should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { var b = stream.CanRead; Assert.Fail("Property access should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { var b = stream.CanSeek; Assert.Fail("Property access should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { var b = stream.CanWrite; Assert.Fail("Property access should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { stream.Flush(); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { var l = stream.Length; Assert.Fail("Property access should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { var l = stream.Position; Assert.Fail("Property access should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { stream.Position = 12345L; Assert.Fail("Property access should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { stream.Read(buffer, 0, 8192); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { stream.Seek(0, SeekOrigin.Current); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { stream.SetLength(12345L); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			try { stream.Write(buffer, 0, 8192); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			// Ensure that an underlying stream is disposed of properly if leaveopen is not set
			MemoryStream ms = new MemoryStream(s_sampledata);
			using (GzipWriter compressor = new GzipWriter(ms, CompressionLevel.Fastest)) { }
			try { ms.Write(buffer, 0, 8192); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ObjectDisposedException)); }

			// Ensure that an underlying stream is not disposed of if leaveopen is set
			ms = new MemoryStream(s_sampledata);
			using (GzipWriter compressor = new GzipWriter(ms, CompressionLevel.Fastest, true)) { }
			ms.Write(buffer, 0, 8192);
			ms.Dispose();
		}

		[TestMethod(), TestCategory("Gzip")]
		public void Gzip_BaseStream()
		{
			using (MemoryStream source = new MemoryStream())
			{
				using (GzipWriter stream = new GzipWriter(source, CompressionLevel.Optimal))
				{
					Assert.IsNotNull(stream.BaseStream);
					Assert.AreSame(source, stream.BaseStream);
				}
			}
		}

		[TestMethod(), TestCategory("Gzip")]
		public void Gzip_CanRead()
		{
			// Verify behavior of a compression stream
			using (GzipWriter stream = new GzipWriter(new MemoryStream()))
			{
				Assert.IsFalse(stream.CanRead);
			}

			// Verify behavior of a decompression stream
			using (GzipReader stream = new GzipReader(new MemoryStream(s_sampledata)))
			{
				Assert.IsTrue(stream.CanRead);
			}
		}

		[TestMethod(), TestCategory("Gzip")]
		public void Gzip_CanSeek()
		{
			// Verify behavior of a compression stream
			using (GzipWriter stream = new GzipWriter(new MemoryStream()))
			{
				Assert.IsFalse(stream.CanSeek);
			}

			// Verify behavior of a decompression stream
			using (GzipReader stream = new GzipReader(new MemoryStream(s_sampledata)))
			{
				Assert.IsFalse(stream.CanSeek);
			}
		}

		[TestMethod(), TestCategory("Gzip")]
		public void Gzip_CanWrite()
		{
			// Verify behavior of a compression stream
			using (GzipWriter stream = new GzipWriter(new MemoryStream()))
			{
				Assert.IsTrue(stream.CanWrite);
			}

			// Verify behavior of a decompression stream
			using (GzipReader stream = new GzipReader(new MemoryStream(s_sampledata)))
			{
				Assert.IsFalse(stream.CanWrite);
			}
		}

		[TestMethod(), TestCategory("Gzip")]
		public void Gzip_Length()
		{
			// Verify behavior of a compression stream
			using (GzipWriter stream = new GzipWriter(new MemoryStream()))
			{
				try { var l = stream.Length; Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}

			// Verify behavior of a decompression stream
			using (GzipReader stream = new GzipReader(new MemoryStream(s_sampledata)))
			{
				try { var l = stream.Length; Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}
		}

		[TestMethod(), TestCategory("Gzip")]
		public void Gzip_Flush()
		{
			// Verify behavior of flushing a compression stream
			using (MemoryStream compressed = new MemoryStream())
			{
				using (GzipWriter stream = new GzipWriter(compressed, true))
				{
					stream.Write(s_sampledata, 0, s_sampledata.Length);

					// Get the unflushed length of the compressed stream and flush it
					long unflushed = compressed.Length;
					stream.Flush();

					// The expectation is that the output stream will be longer after the flush
					long flushedonce = compressed.Length;
					Assert.IsTrue(compressed.Length > unflushed);

					// Flushing the same data a second time should not have any impact at all
					stream.Flush();
					Assert.AreEqual(compressed.Length, flushedonce);

					// The stream should still be writable after a flush operation
					stream.Write(s_sampledata, 0, s_sampledata.Length / 10);
				}
			}

			// Verify behavior of flushing a decompression stream
			using (GzipReader stream = new GzipReader(new MemoryStream(s_sampledata)))
			{
				// Flush has no effect on decompression streams, just ensure it doesn't throw
				stream.Flush();
			}
		}

		[TestMethod(), TestCategory("Gzip")]
		public void Gzip_Position()
		{
			// Start with a MemoryStream created from the sample data
			using (MemoryStream source = new MemoryStream(s_sampledata))
			{
				using (MemoryStream dest = new MemoryStream())
				{
					// Test a compression stream
					using (GzipWriter compressor = new GzipWriter(dest, CompressionLevel.Optimal, true))
					{
						// The stream should report position zero prior to compression
						Assert.AreEqual(0L, compressor.Position);
						source.CopyTo(compressor);

						// The stream should report non-zero after compression
						Assert.AreNotEqual(0L, compressor.Position);

						// Attempting to set the position on the stream should throw
						try { compressor.Position = 12345L; Assert.Fail("Property should have thrown an exception"); }
						catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
					}

					source.SetLength(0);            // Clear the source stream
					dest.Position = 0;              // Reset the destination stream

					// Test a decompression stream
					using (GzipReader decompressor = new GzipReader(dest, true))
					{
						// The stream should report position zero prior to compression
						Assert.AreEqual(0L, decompressor.Position);
						decompressor.CopyTo(source);

						// The stream should report non-zero after compression
						Assert.AreNotEqual(0L, decompressor.Position);

						// Attempting to set the position on the stream should throw
						try { decompressor.Position = 12345L; Assert.Fail("Property should have thrown an exception"); }
						catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
					}
				}
			}
		}

		[TestMethod(), TestCategory("Gzip")]
		public void Gzip_Read()
		{
			byte[] buffer = new byte[8192];         // 8KiB data buffer

			using (MemoryStream compressed = new MemoryStream())
			{
				// Start with a compressed MemoryStream created from the sample data
				using (GzipWriter compressor = new GzipWriter(compressed, CompressionLevel.Optimal, true))
				{
					compressor.Write(s_sampledata, 0, s_sampledata.Length);
					compressor.Flush();
				}

				// Create a decompressor to test some of the error cases
				using (GzipReader decompressor = new GzipReader(compressed, true))
				{
					// Send in some bum arguments to Read() to check they are caught
					try { decompressor.Read(null, 0, 0); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

					try { decompressor.Read(buffer, -1, 0); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

					try { decompressor.Read(buffer, 0, -1); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

					try { decompressor.Read(buffer, 0, buffer.Length + 1024); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentException)); }

					// Attempting to read from the end of the compressed stream should throw an InvalidDataException
					try { decompressor.Read(buffer, 0, 8192); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(InvalidDataException)); }

					// Attempting to read from the middle of the compressed stream should throw a GzipException
					compressed.Position = compressed.Position / 2;
					try { decompressor.Read(buffer, 0, 8192); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(GzipException)); }

					// The decompression stream is trashed at this point since the input buffer was filled
					// with data from the middle.  Thought about a special case handler for that, but it's
					// a fringe case.  Verify that the stream is indeed trashed ...
					compressed.Position = 0;
					try { decompressor.Read(buffer, 0, 8192); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(GzipException)); }
				}

				// Create a new decompressor against the same stream and make sure it doesn't throw
				using (GzipReader decompressor = new GzipReader(compressed, true))
				{
					while (decompressor.Read(buffer, 0, 8192) != 0) { }
				}
			}
		}

		[TestMethod(), TestCategory("Gzip")]
		public void Gzip_Seek()
		{
			// Verify behavior of a compression stream
			using (GzipWriter stream = new GzipWriter(new MemoryStream()))
			{
				try { stream.Seek(50, SeekOrigin.Begin); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }

				try { stream.Seek(0, SeekOrigin.Current); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }

				try { stream.Seek(-50, SeekOrigin.End); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}

			// Verify behavior of a decompression stream
			using (GzipReader stream = new GzipReader(new MemoryStream(s_sampledata)))
			{
				try { stream.Seek(50, SeekOrigin.Begin); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }

				try { stream.Seek(0, SeekOrigin.Current); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }

				try { stream.Seek(-50, SeekOrigin.End); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}
		}

		[TestMethod(), TestCategory("Gzip")]
		public void Gzip_SetLength()
		{
			// Verify behavior of a compression stream
			using (GzipWriter stream = new GzipWriter(new MemoryStream()))
			{
				try { stream.SetLength(12345L); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}

			// Verify behavior of a decompression stream
			using (GzipReader stream = new GzipReader(new MemoryStream(s_sampledata)))
			{
				try { stream.SetLength(12345L); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}
		}

		[TestMethod(), TestCategory("Gzip")]
		public void Gzip_Write()
		{
			byte[] buffer = new byte[8192];         // 8KiB data buffer

			// Compress the sample data using a call to Write directly
			using (MemoryStream compressed = new MemoryStream())
			{
				using (GzipWriter compressor = new GzipWriter(compressed, CompressionLevel.Optimal, true))
				{
					// Send in some bum arguments to Write() to check they are caught
					try { compressor.Write(null, 0, 0); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

					try { compressor.Write(s_sampledata, -1, 0); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

					try { compressor.Write(s_sampledata, 0, -1); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

					try { compressor.Write(s_sampledata, 0, s_sampledata.Length + 1024); Assert.Fail("Method call should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentException)); }

					// Compress the data; there really isn't much that can go wrong with Write() itself
					compressor.Write(s_sampledata, 0, s_sampledata.Length);
					compressor.Flush();
				}
			}
		}

		[TestMethod(), TestCategory("Gzip")]
		public void Gzip_Encoder()
		{
			// The GzipEncoder is just a wrapper around GzipWriter that provides 
			// more complete control over the compression/encoder parameters
			GzipEncoder encoder = new GzipEncoder();

			// Check the default values
			Assert.AreEqual(65536, encoder.BufferSize);
			Assert.AreEqual(GzipCompressionLevel.Default, encoder.CompressionLevel);

			// Set some bad values and ensure they are caught by the encoder property setters
			try { encoder.BufferSize = -1; Assert.Fail("Property should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			try { encoder.CompressionLevel = new GzipCompressionLevel(-2); Assert.Fail("Property should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			try { encoder.CompressionLevel = new GzipCompressionLevel(10); Assert.Fail("Property should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			// Check all of the Encoder methods work and encode as expected
			byte[] expected, actual;
			using (MemoryStream ms = new MemoryStream())
			{
				using (var writer = new GzipWriter(ms, true)) writer.Write(s_sampledata, 0, s_sampledata.Length);
				expected = ms.ToArray();
			}

			actual = encoder.Encode(s_sampledata);
			Assert.IsTrue(Enumerable.SequenceEqual(expected, actual));

			actual = encoder.Encode(new MemoryStream(s_sampledata));
			Assert.IsTrue(Enumerable.SequenceEqual(expected, actual));

			actual = encoder.Encode(s_sampledata, 0, s_sampledata.Length);
			Assert.IsTrue(Enumerable.SequenceEqual(expected, actual));

			using (MemoryStream dest = new MemoryStream())
			{
				encoder.Encode(s_sampledata, dest);
				Assert.IsTrue(Enumerable.SequenceEqual(expected, dest.ToArray()));
			}

			using (MemoryStream dest = new MemoryStream())
			{
				encoder.Encode(new MemoryStream(s_sampledata), dest);
				Assert.IsTrue(Enumerable.SequenceEqual(expected, dest.ToArray()));
			}

			using (MemoryStream dest = new MemoryStream())
			{
				encoder.Encode(s_sampledata, 0, s_sampledata.Length, dest);
				Assert.IsTrue(Enumerable.SequenceEqual(expected, dest.ToArray()));
			}
		}
	}
}
