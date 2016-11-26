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

using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace zuki.io.compression.test
{
	[TestClass()]
	public class TestXz
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

		[TestMethod(), TestCategory("Xz")]
		public void Xz_DecompressExternal()
		{
			// Decompress a stream created externally to this library
			using (XzReader reader = new XzReader(Assembly.GetExecutingAssembly().GetManifestResourceStream("zuki.io.compression.test.thethreemusketeers.xz")))
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

		[TestMethod(), TestCategory("Xz")]
		public void Xz_Stub()
		{
			Assert.Fail("not implemented");
		}
	}
}
