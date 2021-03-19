using System;
using System.Collections.Generic;
using System.Text;

namespace HelloWorld20
{
    class MyClass
    {

        public void PrintThis<T>(T asdf) 
        {
            Console.WriteLine(asdf);
        }
    }
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Hello world!");
            Console.WriteLine("Hello world another time just to be sure!");
            var m = new MyClass();
            m.PrintThis("aaaaaaaaaaaaaaaaa");
        }
    }

}
