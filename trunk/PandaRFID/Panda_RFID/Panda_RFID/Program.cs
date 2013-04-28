using System;
using System.Threading;

using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
using System.IO.Ports;
using System.Text;

using GHIElectronics.NETMF.FEZ;

namespace Panda_RFID
{
    public class Program
    {
        public static void Main()
        {
            
            SerialPort UART = new SerialPort("COM1", 9600);
            SPI.Configuration config = new SPI.Configuration((Cpu.Pin)FEZ_Pin.Digital.Di10, false, 0, 0, true, true, 250, SPI.SPI_module.SPI1);
            SPI Spi = new SPI(config);
            UART.Open();
            string Datoreaded = "";
            byte[] datain = new byte[1];
            byte[] dataout = new byte[1];
            while (true)
            {
                // Sleep for 500 milliseconds
                Thread.Sleep(500);
                
                dataout[0] = (byte)0x55;
                datain[0] = (byte)0;
                Spi.WriteRead(dataout, datain);
                Datoreaded = datain[0].ToString();
                Datoreaded += "\n\r";
                byte[] buffer = Encoding.UTF8.GetBytes(Datoreaded);
                UART.Write(buffer, 0, buffer.Length);
                
            }
        }

    }
}
