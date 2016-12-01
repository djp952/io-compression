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
	public class TestLzma
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

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_CompressExternal()
		{
			// This method generates an output file that can be tested externally; "thethreemusketeers.txt" is
			// set to Copy Always to the output directory, it can be diffed after running the external tool
			LzmaEncoder encoder = new LzmaEncoder();
			using (var outfile = File.Create(Path.Combine(Environment.CurrentDirectory, "thethreemusketeers.lzma")))
			{
				encoder.Encode(s_sampledata, outfile);
				outfile.Flush();
			}
		}

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_DecompressExternal()
		{
			// Decompress a stream created externally to this library
			using (LzmaReader reader = new LzmaReader(Assembly.GetExecutingAssembly().GetManifestResourceStream("zuki.io.compression.test.thethreemusketeers.lzma")))
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

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_CompressDecompress()
		{
			// Start with a MemoryStream created from the sample data
			using (MemoryStream source = new MemoryStream(s_sampledata))
			{
				using (MemoryStream dest = new MemoryStream())
				{
					// Compress the data into the destination memory stream instance
					LzmaEncoder encoder = new LzmaEncoder();
					encoder.Encode(source, dest);

					// The compressed data should be smaller than the source data
					Assert.IsTrue(dest.Length < source.Length);

					source.SetLength(0);            // Clear the source stream
					dest.Position = 0;              // Reset the destination stream

					// Decompress the data back into the source memory stream
					using (LzmaReader decompressor = new LzmaReader(dest, true)) decompressor.CopyTo(source);

					// Ensure that the original data has been restored
					Assert.AreEqual(source.Length, s_sampledata.Length);
					Assert.IsTrue(s_sampledata.SequenceEqual(source.ToArray()));
				}
			}
		}

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_Dispose()
		{
			byte[] buffer = new byte[8192];         // 8KiB data buffer

			// Create a memorystream to hold the compressed sample data
			using (MemoryStream ms = new MemoryStream())
			{
				LzmaEncoder encoder = new LzmaEncoder();
				encoder.Encode(s_sampledata, ms);

				// Create a decompression stream and immediately dispose of it
				LzmaReader stream = new LzmaReader(ms);
				stream.Dispose();

				// Test double dispose
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
			}
		}

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_BaseStream()
		{
			using (MemoryStream dest = new MemoryStream())
			{
				LzmaEncoder encoder = new LzmaEncoder();
				encoder.Encode(s_sampledata, dest);
				using (LzmaReader stream = new LzmaReader(dest))
				{
					Assert.IsNotNull(stream.BaseStream);
					Assert.AreSame(dest, stream.BaseStream);
				}
			}
		}

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_CanRead()
		{
			// Verify behavior of a decompression stream
			using (LzmaReader stream = new LzmaReader(new MemoryStream(s_sampledata)))
			{
				Assert.IsTrue(stream.CanRead);
			}
		}

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_CanSeek()
		{
			// Verify behavior of a decompression stream
			using (LzmaReader stream = new LzmaReader(new MemoryStream(s_sampledata)))
			{
				Assert.IsFalse(stream.CanSeek);
			}
		}

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_CanWrite()
		{
			// Verify behavior of a decompression stream
			using (LzmaReader stream = new LzmaReader(new MemoryStream(s_sampledata)))
			{
				Assert.IsFalse(stream.CanWrite);
			}
		}

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_Length()
		{
			// Verify behavior of a decompression stream
			using (LzmaReader stream = new LzmaReader(new MemoryStream(s_sampledata)))
			{
				try { var l = stream.Length; Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}
		}

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_Flush()
		{
			// Verify behavior of flushing a decompression stream
			using (LzmaReader stream = new LzmaReader(new MemoryStream(s_sampledata)))
			{
				// Flush has no effect on decompression streams, just ensure it doesn't throw
				stream.Flush();
			}
		}

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_Position()
		{
			// Start with a MemoryStream created from the sample data
			using (MemoryStream compressed = new MemoryStream(s_sampledata))
			{
				LzmaEncoder encoder = new LzmaEncoder();
				encoder.Encode(s_sampledata, compressed);

				compressed.Position = 0;
				using (LzmaReader reader = new LzmaReader(compressed))
				{
					// Attempting to set the position on the stream should throw
					try { reader.Position = 12345L; Assert.Fail("Property should have thrown an exception"); }
					catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }

					// Read some data from the stream, position should be non-zero
					byte[] buffer = new byte[8192];
					reader.Read(buffer, 0, 8192);
					Assert.AreNotEqual(0L, reader.Position);
				}
			}
		}

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_Read()
		{
			byte[] buffer = new byte[8192];         // 8KiB data buffer

			using (MemoryStream compressed = new MemoryStream())
			{
				LzmaEncoder encoder = new LzmaEncoder();
				encoder.Encode(s_sampledata, compressed);
				compressed.Flush();

				// Check the constructor for ArgumentNullException while we're here
				try { using (LzmaReader decompressor = new LzmaReader(null, false)) { }; Assert.Fail("Constructor should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

				// Create a decompressor to test some of the error cases
				using (LzmaReader decompressor = new LzmaReader(compressed, true))
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
				}

				// Create a new decompressor against the same stream and make sure it doesn't throw
				compressed.Position = 0;
				using (LzmaReader decompressor = new LzmaReader(compressed, true))
				{
					// Reading zero bytes should not throw an exception
					decompressor.Read(buffer, 0, 0);

					while (decompressor.Read(buffer, 0, 8192) != 0) { }
				}
			}
		}

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_Seek()
		{
			// Verify behavior of a decompression stream
			using (LzmaReader stream = new LzmaReader(new MemoryStream(s_sampledata)))
			{
				try { stream.Seek(50, SeekOrigin.Begin); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }

				try { stream.Seek(0, SeekOrigin.Current); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }

				try { stream.Seek(-50, SeekOrigin.End); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}
		}

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_SetLength()
		{
			// Verify behavior of a decompression stream
			using (LzmaReader stream = new LzmaReader(new MemoryStream(s_sampledata)))
			{
				try { stream.SetLength(12345L); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}
		}

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_Write()
		{
			LzmaEncoder encoder = new LzmaEncoder();
			var compressed = encoder.Encode(s_sampledata);
			using (LzmaReader reader = new LzmaReader(new MemoryStream(compressed)))
			{
				byte[] buffer = new byte[2048];
				try { reader.Write(buffer, 0, buffer.Length); Assert.Fail("Method call should have thrown an exception"); }
				catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(NotSupportedException)); }
			}
		}

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_LzmaCompressionLevel()
		{
			// Constructors
			var level = new LzmaCompressionLevel(5);
			Assert.AreEqual(5, level);

			level = new LzmaCompressionLevel(CompressionLevel.Fastest);
			Assert.AreEqual(1, level);

			level = new LzmaCompressionLevel(CompressionLevel.Optimal);
			Assert.AreEqual(9, level);

			try { level = new LzmaCompressionLevel(-1); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			try { level = new LzmaCompressionLevel(14); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			try { level = new LzmaCompressionLevel(CompressionLevel.NoCompression); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			try { level = new LzmaCompressionLevel((CompressionLevel)99); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			// Implicit conversion
			level = 8;
			Assert.AreEqual(8, level);

			// Equality
			Assert.IsTrue(new LzmaCompressionLevel(8) == level);
			Assert.IsFalse(new LzmaCompressionLevel(1) == level);
			Assert.IsTrue(new LzmaCompressionLevel(3) != level);
			Assert.IsFalse(new LzmaCompressionLevel(8) != level);

			object o = new LzmaCompressionLevel(2);
			Assert.IsTrue(o.Equals(new LzmaCompressionLevel(2)));
			Assert.IsFalse(o.Equals(new LzmaCompressionLevel(9)));
			Assert.IsTrue(new LzmaCompressionLevel(2).Equals(new LzmaCompressionLevel(2)));
			Assert.IsFalse(new LzmaCompressionLevel(8).Equals(new LzmaCompressionLevel(2)));
			Assert.IsFalse(o.Equals(null));
			Assert.IsFalse(o.Equals(new LzmaMatchFindPasses(88)));

			// HashCode
			int hash = new LzmaCompressionLevel(CompressionLevel.Fastest).GetHashCode();
			Assert.AreNotEqual(0, hash);

			// ToString
			string s = new LzmaCompressionLevel(9).ToString();
			Assert.IsFalse(String.IsNullOrEmpty(s));
		}

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_LzmaDictionarySize()
		{
			// Constructors
			var size = new LzmaDictionarySize(1 << 24);
			Assert.AreEqual(1 << 24, size);

			try { size = new LzmaDictionarySize(0); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			try { size = new LzmaDictionarySize((1 << 30) + 1); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			// Implicit conversion
			size = 1 << 24;
			Assert.AreEqual(1 << 24, size);

			// Equality
			Assert.IsTrue(new LzmaDictionarySize(1 << 24) == size);
			Assert.IsFalse(new LzmaDictionarySize(1 << 27) == size);
			Assert.IsTrue(new LzmaDictionarySize(1 << 27) != size);
			Assert.IsFalse(new LzmaDictionarySize(1 << 24) != size);

			object o = new LzmaDictionarySize(1 << 25);
			Assert.IsTrue(o.Equals(new LzmaDictionarySize(1 << 25)));
			Assert.IsFalse(o.Equals(new LzmaDictionarySize(1 << 24)));
			Assert.IsTrue(new LzmaDictionarySize(1 << 25).Equals(new LzmaDictionarySize(1 << 25)));
			Assert.IsFalse(new LzmaDictionarySize(1 << 25).Equals(new LzmaDictionarySize(1 << 26)));
			Assert.IsFalse(o.Equals(null));
			Assert.IsFalse(o.Equals(new LzmaMatchFindPasses(88)));

			// HashCode
			int hash = new LzmaDictionarySize(1 << 24).GetHashCode();
			Assert.AreNotEqual(0, hash);

			// ToString
			string s = new LzmaDictionarySize(1 << 25).ToString();
			Assert.IsFalse(String.IsNullOrEmpty(s));
		}

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_LzmaFastBytes()
		{
			// Constructors
			var bytes = new LzmaFastBytes(5);
			Assert.AreEqual(5, bytes);

			try { bytes = new LzmaFastBytes(4); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			try { bytes = new LzmaFastBytes(274); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			// Implicit conversion
			bytes = 8;
			Assert.AreEqual(8, bytes);

			// Equality
			Assert.IsTrue(new LzmaFastBytes(8) == bytes);
			Assert.IsFalse(new LzmaFastBytes(11) == bytes);
			Assert.IsTrue(new LzmaFastBytes(33) != bytes);
			Assert.IsFalse(new LzmaFastBytes(8) != bytes);

			object o = new LzmaFastBytes(22);
			Assert.IsTrue(o.Equals(new LzmaFastBytes(22)));
			Assert.IsFalse(o.Equals(new LzmaFastBytes(9)));
			Assert.IsTrue(new LzmaFastBytes(22).Equals(new LzmaFastBytes(22)));
			Assert.IsFalse(new LzmaFastBytes(8).Equals(new LzmaFastBytes(22)));
			Assert.IsFalse(o.Equals(null));
			Assert.IsFalse(o.Equals(new LzmaMatchFindPasses(88)));

			// HashCode
			int hash = new LzmaFastBytes(123).GetHashCode();
			Assert.AreNotEqual(0, hash);

			// ToString
			string s = new LzmaFastBytes(123).ToString();
			Assert.IsFalse(String.IsNullOrEmpty(s));
		}

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_LzmaHashBytes()
		{
			// Constructors
			var bytes = new LzmaHashBytes(3);
			Assert.AreEqual(3, bytes);

			try { bytes = new LzmaHashBytes(1); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			try { bytes = new LzmaHashBytes(5); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			// Implicit conversion
			bytes = 3;
			Assert.AreEqual(3, bytes);

			// Equality
			Assert.IsTrue(new LzmaHashBytes(3) == bytes);
			Assert.IsFalse(new LzmaHashBytes(4) == bytes);
			Assert.IsTrue(new LzmaHashBytes(2) != bytes);
			Assert.IsFalse(new LzmaHashBytes(3) != bytes);

			object o = new LzmaHashBytes(2);
			Assert.IsTrue(o.Equals(new LzmaHashBytes(2)));
			Assert.IsFalse(o.Equals(new LzmaHashBytes(3)));
			Assert.IsTrue(new LzmaHashBytes(2).Equals(new LzmaHashBytes(2)));
			Assert.IsFalse(new LzmaHashBytes(3).Equals(new LzmaHashBytes(2)));
			Assert.IsFalse(o.Equals(null));
			Assert.IsFalse(o.Equals(new LzmaMatchFindPasses(88)));

			// HashCode
			int hash = new LzmaHashBytes(2).GetHashCode();
			Assert.AreNotEqual(0, hash);

			// ToString
			string s = new LzmaHashBytes(2).ToString();
			Assert.IsFalse(String.IsNullOrEmpty(s));
		}

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_LzmaLiteralContextBits()
		{
			// Constructors
			var bits = new LzmaLiteralContextBits(3);
			Assert.AreEqual(3, bits);

			try { bits = new LzmaLiteralContextBits(-1); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			try { bits = new LzmaLiteralContextBits(9); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			// Implicit conversion
			bits = 3;
			Assert.AreEqual(3, bits);

			// Equality
			Assert.IsTrue(new LzmaLiteralContextBits(3) == bits);
			Assert.IsFalse(new LzmaLiteralContextBits(4) == bits);
			Assert.IsTrue(new LzmaLiteralContextBits(2) != bits);
			Assert.IsFalse(new LzmaLiteralContextBits(3) != bits);

			object o = new LzmaLiteralContextBits(2);
			Assert.IsTrue(o.Equals(new LzmaLiteralContextBits(2)));
			Assert.IsFalse(o.Equals(new LzmaLiteralContextBits(3)));
			Assert.IsTrue(new LzmaLiteralContextBits(2).Equals(new LzmaLiteralContextBits(2)));
			Assert.IsFalse(new LzmaLiteralContextBits(3).Equals(new LzmaLiteralContextBits(2)));
			Assert.IsFalse(o.Equals(null));
			Assert.IsFalse(o.Equals(new LzmaMatchFindPasses(88)));

			// HashCode
			int hash = new LzmaLiteralContextBits(2).GetHashCode();
			Assert.AreNotEqual(0, hash);

			// ToString
			string s = new LzmaLiteralContextBits(2).ToString();
			Assert.IsFalse(String.IsNullOrEmpty(s));
		}

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_LzmaLiteralPositionBits()
		{
			// Constructors
			var bits = new LzmaLiteralPositionBits(3);
			Assert.AreEqual(3, bits);

			try { bits = new LzmaLiteralPositionBits(-1); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			try { bits = new LzmaLiteralPositionBits(5); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			// Implicit conversion
			bits = 3;
			Assert.AreEqual(3, bits);

			// Equality
			Assert.IsTrue(new LzmaLiteralPositionBits(3) == bits);
			Assert.IsFalse(new LzmaLiteralPositionBits(4) == bits);
			Assert.IsTrue(new LzmaLiteralPositionBits(2) != bits);
			Assert.IsFalse(new LzmaLiteralPositionBits(3) != bits);

			object o = new LzmaLiteralPositionBits(2);
			Assert.IsTrue(o.Equals(new LzmaLiteralPositionBits(2)));
			Assert.IsFalse(o.Equals(new LzmaLiteralPositionBits(3)));
			Assert.IsTrue(new LzmaLiteralPositionBits(2).Equals(new LzmaLiteralPositionBits(2)));
			Assert.IsFalse(new LzmaLiteralPositionBits(3).Equals(new LzmaLiteralPositionBits(2)));
			Assert.IsFalse(o.Equals(null));
			Assert.IsFalse(o.Equals(new LzmaMatchFindPasses(88)));

			// HashCode
			int hash = new LzmaLiteralPositionBits(2).GetHashCode();
			Assert.AreNotEqual(0, hash);

			// ToString
			string s = new LzmaLiteralPositionBits(2).ToString();
			Assert.IsFalse(String.IsNullOrEmpty(s));
		}

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_LzmaMatchFindPasses()
		{
			// Constructors
			var passes = new LzmaMatchFindPasses(3);
			Assert.AreEqual(3, passes);

			try { passes = new LzmaMatchFindPasses(-1); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			try { passes = new LzmaMatchFindPasses((1 << 30) + 1); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			// Implicit conversion
			passes = 3;
			Assert.AreEqual(3, passes);

			// Equality
			Assert.IsTrue(new LzmaMatchFindPasses(3) == passes);
			Assert.IsFalse(new LzmaMatchFindPasses(4) == passes);
			Assert.IsTrue(new LzmaMatchFindPasses(2) != passes);
			Assert.IsFalse(new LzmaMatchFindPasses(3) != passes);

			object o = new LzmaMatchFindPasses(2);
			Assert.IsTrue(o.Equals(new LzmaMatchFindPasses(2)));
			Assert.IsFalse(o.Equals(new LzmaMatchFindPasses(3)));
			Assert.IsTrue(new LzmaMatchFindPasses(2).Equals(new LzmaMatchFindPasses(2)));
			Assert.IsFalse(new LzmaMatchFindPasses(3).Equals(new LzmaMatchFindPasses(2)));
			Assert.IsFalse(o.Equals(null));
			Assert.IsFalse(o.Equals(new LzmaCompressionLevel(5)));

			// HashCode
			int hash = new LzmaMatchFindPasses(2).GetHashCode();
			Assert.AreNotEqual(0, hash);

			// ToString
			string s = new LzmaMatchFindPasses(2).ToString();
			Assert.IsFalse(String.IsNullOrEmpty(s));
		}

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_LzmaPositionBits()
		{
			// Constructors
			var bits = new LzmaPositionBits(3);
			Assert.AreEqual(3, bits);

			try { bits = new LzmaPositionBits(-1); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			try { bits = new LzmaPositionBits(5); Assert.Fail("Constructor should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentOutOfRangeException)); }

			// Implicit conversion
			bits = 3;
			Assert.AreEqual(3, bits);

			// Equality
			Assert.IsTrue(new LzmaPositionBits(3) == bits);
			Assert.IsFalse(new LzmaPositionBits(4) == bits);
			Assert.IsTrue(new LzmaPositionBits(2) != bits);
			Assert.IsFalse(new LzmaPositionBits(3) != bits);

			object o = new LzmaPositionBits(2);
			Assert.IsTrue(o.Equals(new LzmaPositionBits(2)));
			Assert.IsFalse(o.Equals(new LzmaPositionBits(3)));
			Assert.IsTrue(new LzmaPositionBits(2).Equals(new LzmaPositionBits(2)));
			Assert.IsFalse(new LzmaPositionBits(3).Equals(new LzmaPositionBits(2)));
			Assert.IsFalse(o.Equals(null));
			Assert.IsFalse(o.Equals(new LzmaMatchFindPasses(88)));

			// HashCode
			int hash = new LzmaPositionBits(2).GetHashCode();
			Assert.AreNotEqual(0, hash);

			// ToString
			string s = new LzmaPositionBits(2).ToString();
			Assert.IsFalse(String.IsNullOrEmpty(s));
		}

		[TestMethod(), TestCategory("Lzma")]
		public void Lzma_Encoder()
		{
			LzmaEncoder encoder = new LzmaEncoder();

			// Check the default values
			Assert.AreEqual(LzmaCompressionLevel.Default, encoder.CompressionLevel);
			Assert.AreEqual(LzmaCompressionMode.Default, encoder.CompressionMode);
			Assert.AreEqual(LzmaDictionarySize.Default, encoder.DictionarySize);
			Assert.AreEqual(LzmaFastBytes.Default, encoder.FastBytes);
			Assert.AreEqual(LzmaHashBytes.Default, encoder.HashBytes);
			Assert.AreEqual(LzmaLiteralContextBits.Default, encoder.LiteralContextBits);
			Assert.AreEqual(LzmaLiteralPositionBits.Default, encoder.LiteralPositionBits);
			Assert.AreEqual(LzmaMatchFindMode.Default, encoder.MatchFindMode);
			Assert.AreEqual(LzmaMatchFindPasses.Default, encoder.MatchFindPasses);
			Assert.AreEqual(LzmaPositionBits.Default, encoder.PositionBits);
			Assert.AreEqual(true, encoder.UseMultipleThreads);
			Assert.AreEqual(false, encoder.WriteEndMark);

			// Set and reset encoder parameters to exercise the property setters
			encoder.CompressionLevel = LzmaCompressionLevel.Fastest;
			Assert.AreEqual(LzmaCompressionLevel.Fastest, encoder.CompressionLevel);
			encoder.CompressionLevel = LzmaCompressionLevel.Default;
			Assert.AreEqual(LzmaCompressionLevel.Default, encoder.CompressionLevel);

			encoder.CompressionMode = LzmaCompressionMode.Fast;
			Assert.AreEqual(LzmaCompressionMode.Fast, encoder.CompressionMode);
			encoder.CompressionMode = LzmaCompressionMode.Default;
			Assert.AreEqual(LzmaCompressionMode.Default, encoder.CompressionMode);

			encoder.DictionarySize = LzmaDictionarySize.Maximum;
			Assert.AreEqual(LzmaDictionarySize.Maximum, encoder.DictionarySize);
			encoder.DictionarySize = LzmaDictionarySize.Default;
			Assert.AreEqual(LzmaDictionarySize.Default, encoder.DictionarySize);

			encoder.FastBytes = LzmaFastBytes.Maximum;
			Assert.AreEqual(LzmaFastBytes.Maximum, encoder.FastBytes);
			encoder.FastBytes = LzmaFastBytes.Default;
			Assert.AreEqual(LzmaFastBytes.Default, encoder.FastBytes);

			encoder.HashBytes = LzmaHashBytes.Maximum;
			Assert.AreEqual(LzmaHashBytes.Maximum, encoder.HashBytes);
			encoder.HashBytes = LzmaHashBytes.Default;
			Assert.AreEqual(LzmaHashBytes.Default, encoder.HashBytes);

			encoder.LiteralContextBits = LzmaLiteralContextBits.Maximum;
			Assert.AreEqual(LzmaLiteralContextBits.Maximum, encoder.LiteralContextBits);
			encoder.LiteralContextBits = LzmaLiteralContextBits.Default;
			Assert.AreEqual(LzmaLiteralContextBits.Default, encoder.LiteralContextBits);

			encoder.LiteralPositionBits = LzmaLiteralPositionBits.Maximum;
			Assert.AreEqual(LzmaLiteralPositionBits.Maximum, encoder.LiteralPositionBits);
			encoder.LiteralPositionBits = LzmaLiteralPositionBits.Default;
			Assert.AreEqual(LzmaLiteralPositionBits.Default, encoder.LiteralPositionBits);

			encoder.MatchFindMode = LzmaMatchFindMode.HashChain;
			Assert.AreEqual(LzmaMatchFindMode.HashChain, encoder.MatchFindMode);
			encoder.MatchFindMode = LzmaMatchFindMode.Default;
			Assert.AreEqual(LzmaMatchFindMode.Default, encoder.MatchFindMode);

			encoder.MatchFindPasses = LzmaMatchFindPasses.Maximum;
			Assert.AreEqual(LzmaMatchFindPasses.Maximum, encoder.MatchFindPasses);
			encoder.MatchFindPasses = LzmaMatchFindPasses.Default;
			Assert.AreEqual(LzmaMatchFindPasses.Default, encoder.MatchFindPasses);

			encoder.PositionBits = LzmaPositionBits.Maximum;
			Assert.AreEqual(LzmaPositionBits.Maximum, encoder.PositionBits);
			encoder.PositionBits = LzmaPositionBits.Default;
			Assert.AreEqual(LzmaPositionBits.Default, encoder.PositionBits);

			encoder.UseMultipleThreads = false;
			Assert.AreEqual(false, encoder.UseMultipleThreads);
			encoder.UseMultipleThreads = true;
			Assert.AreEqual(true, encoder.UseMultipleThreads);

			encoder.WriteEndMark = true;
			Assert.AreEqual(true, encoder.WriteEndMark);
			encoder.WriteEndMark = false;
			Assert.AreEqual(false, encoder.WriteEndMark);

			// Check all of the Encoder methods work and encode as expected
			byte[] expected, expectedstreamed, actual;

			// There are 2 expected results since when the length of the input
			// is known it's encoded into the compressed stream
			using (MemoryStream ms = new MemoryStream())
			{
				encoder.Encode(s_sampledata, ms);
				ms.Flush();
				expected = ms.ToArray();
			}

			using (MemoryStream ms = new MemoryStream())
			{
				encoder.Encode(new MemoryStream(s_sampledata), ms);
				ms.Flush();
				expectedstreamed = ms.ToArray();
			}

			// Check parameter validations
			try { actual = encoder.Encode((byte[])null); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

			try { actual = encoder.Encode((Stream)null); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

			try { actual = encoder.Encode(null, 0, 0); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

			try { encoder.Encode(s_sampledata, null); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

			try { encoder.Encode((byte[])null, new MemoryStream()); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

			try { encoder.Encode((byte[])null, 0, 0, new MemoryStream()); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

			try { encoder.Encode(s_sampledata, 0, s_sampledata.Length, (Stream)null); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

			try { encoder.Encode((Stream)null, new MemoryStream()); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

			try { encoder.Encode(new MemoryStream(), (Stream)null); Assert.Fail("Method call should have thrown an exception"); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(ArgumentNullException)); }

			// Check actual encoding operations
			actual = encoder.Encode(s_sampledata);
			Assert.IsTrue(Enumerable.SequenceEqual(expected, actual));                      // length is known

			actual = encoder.Encode(new MemoryStream(s_sampledata));
			Assert.IsTrue(Enumerable.SequenceEqual(expectedstreamed, actual));              // length is not known

			actual = encoder.Encode(s_sampledata, 0, s_sampledata.Length);                  // length is known
			Assert.IsTrue(Enumerable.SequenceEqual(expected, actual));

			using (MemoryStream dest = new MemoryStream())
			{
				encoder.Encode(s_sampledata, dest);
				Assert.IsTrue(Enumerable.SequenceEqual(expected, dest.ToArray()));          // length is known
			}

			using (MemoryStream dest = new MemoryStream())
			{
				encoder.Encode(new MemoryStream(s_sampledata), dest);
				Assert.IsTrue(Enumerable.SequenceEqual(expectedstreamed, dest.ToArray()));  // length is not known
			}

			using (MemoryStream dest = new MemoryStream())
			{
				encoder.Encode(s_sampledata, 0, s_sampledata.Length, dest);
				Assert.IsTrue(Enumerable.SequenceEqual(expected, dest.ToArray()));          // length is known
			}
		}
	}
}
