using System;
using lpubsppop01.REPLReaderWriter;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace REPLReaderWriterTestsManaged
{
    [TestClass]
    public class REPLManagerTests
    {
        [TestMethod]
        public void Managed_TestWithPython()
        {
            int processID = REPLManager.Start("C:/tools/Anaconda3/python.exe", "Python");
            Assert.IsTrue(processID > 0);
            REPLManager.SetTimeout(processID, 3000);

            REPLManager.WriteLine(processID, "1 + 1");
            REPLManager.WaitFor(processID, ".*");
            string outputText = REPLManager.ReadOutputLine(processID);
            Assert.IsTrue(outputText == "2");
            REPLManager.WaitForPrompt(processID);
            string errorText = REPLManager.ReadErrorLine(processID);
            Assert.AreEqual(null, errorText);

            REPLManager.Stop(processID);
        }
    }
}
