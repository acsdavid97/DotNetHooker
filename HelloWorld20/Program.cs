using System;
using System.Collections.Generic;
using System.Text;

namespace HelloWorld20
{
    class Program
    {
        static void MyWriteLine(bool a, string str, byte[] myarr)
        {
            Console.WriteLine(str + a);
        }
        static void Main(string[] args)
        {
            MyWriteLine(true, "Hello world!", new byte[] { 0x12, 0x34 });
        }
    }

}
