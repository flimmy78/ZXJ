﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using WFNetLib.TCP;
using System.Net;
using WFNetLib.PacketProc;
using WFNetLib;

namespace 地声数据监测
{
    public partial class Form2 : Form
    {
        TCPAsyncServer tcpAsyncServer;
        public Form2(TCPAsyncServer _tcpAsyncServer)
        {
            InitializeComponent();
            tcpAsyncServer = _tcpAsyncServer;
        }

        private void Form2_Load(object sender, EventArgs e)
        {
             tcpAsyncServer.ErrorServerEvent += new TCPErrorEvent(tcpAsyncServer_ErrorServer);
             tcpAsyncServer.ReceiveServerEvent += new TCPReceiveEvent(tcpAsyncServer_ReceiveServerEvent);

             numericUpDown3.Value = (decimal)Form1.rioCur;

             if (Form1.bPower1)
                 rbp1on.Checked = true;
             else
                 rbp1off.Checked = true;

             if (Form1.bPower2)
                 rbp2on.Checked = true;
             else
                 rbp2off.Checked = true;


             if (Form1.bIO[0] != 0)
                 rb1h.Checked = true;
             else
                 rb1l.Checked = true;

             if (Form1.bIO[1] != 0)
                 rb2h.Checked = true;
             else
                 rb2l.Checked = true;

             if (Form1.bIO[2] != 0)
                 rb3h.Checked = true;
             else
                 rb3l.Checked = true;

             if (Form1.bIO[3] != 0)
                 rb4h.Checked = true;
             else
                 rb4l.Checked = true;

             if (Form1.bIO[4] != 0)
                 rb5h.Checked = true;
             else
                 rb5l.Checked = true;

             if (Form1.bIO[5] != 0)
                 rb6h.Checked = true;
             else
                 rb6l.Checked = true;
        }
        void NetLog(string str)
        {
            textBox1.AppendText(str);
            WFNetLib.Log.TextLog.AddTextLog(str);
        }
        private void tcpAsyncServer_ReceiveServerEvent(object sender, ReceiveServerEventArgs e)
        {
            IPEndPoint ip = (IPEndPoint)e.Client.ClientSocket.RemoteEndPoint;
            //if (ip.Address.Equals(Form1.mcuSocketIP.Address))
			if (Form1.mcuClientContext != null && e.Client == Form1.mcuClientContext)
			{
                this.Invoke((EventHandler)(delegate
                {
                    CP1616_NoAddr_Packet rx = (CP1616_NoAddr_Packet)e.netPacket;
                    if (rx.Header.Command == 0x02)//读取网络参数
                    {
                        tabControl1.Enabled = true;
                        timer1.Enabled = false;
                        byte[] bIP=new byte[4];
                        for(int i=0;i<4;i++)
                        {
                            bIP[i]=rx.Data[8+i];
                        }
                        IPAddress mcuIP = new IPAddress(bIP);
                        textBox2.Text = mcuIP.ToString();

                        for (int i = 0; i < 4; i++)
                        {
                            bIP[i] = rx.Data[12 + i];
                        }
                        mcuIP = new IPAddress(bIP);
                        textBox3.Text = mcuIP.ToString();

                        for (int i = 0; i < 4; i++)
                        {
                            bIP[i] = rx.Data[16 + i];
                        }
                        mcuIP = new IPAddress(bIP);
                        textBox4.Text = mcuIP.ToString();

                        for (int i = 0; i < 4; i++)
                        {
                            bIP[i] = rx.Data[24 + i];
                        }
                        mcuIP = new IPAddress(bIP);
                        textBox5.Text = mcuIP.ToString();

                        numericUpDown2.Value = BytesOP.MakeShort(rx.Data[0x1d], rx.Data[0x1c]);

                        numericUpDown1.Value = BytesOP.MakeShort(rx.Data[0x21], rx.Data[0x20]);
                    }
                    else if (rx.Header.Command == 0x03)//设定网络参数
                    {
                        tabControl1.Enabled = true;
                        timer1.Enabled = false;
                        MessageBox.Show("设定网络参数成功");
                    }
                    else if (rx.Header.Command == 0x04)//设定光源
                    {
                        tabControl1.Enabled = true;
                        timer1.Enabled = false;
                        if (rx.Data[0] != 0)
                            MessageBox.Show("设定光源电流成功");
                        else
                            MessageBox.Show("设定光源电流失败");
                    }
                    else if (rx.Header.Command == 0x05)//设定备用IO成功
                    {
                        tabControl1.Enabled = true;
                        timer1.Enabled = false;
                        MessageBox.Show("设定备用IO成功");
                    }
                    else if (rx.Header.Command == 0x06)//设定备用IO成功
                    {
                        tabControl1.Enabled = true;
                        timer1.Enabled = false;
                        MessageBox.Show("设定重启成功");
                    }
                    else if (rx.Header.Command == 0x07)//设定备用IO成功
                    {
                        tabControl1.Enabled = true;
                        timer1.Enabled = false;
                        MessageBox.Show("电源设定成功");
                    }
//                     if (bSave)
//                     {
//                         NetLog(DateTime.Now.ToLongTimeString() + "   :   ");
//                         NetLog("收到单片机采集板数据" + e.Client.clientEndPoint.ToString() + ":");
//                         NetLog(WFNetLib.StringFunc.StringsFunction.byteToHexStr(rx.Data, " "));
//                         NetLog("\r\n");
//                     }
//                     else
                    if (rx.Header.Command != 0x01)
                    {
                        textBox1.AppendText(DateTime.Now.ToLongTimeString() + "   :   ");
                        textBox1.AppendText("收到单片机采集板数据" + e.Client.clientEndPoint.ToString() + ":");
                        textBox1.AppendText(WFNetLib.StringFunc.StringsFunction.byteToHexStr(rx.Data, " "));
                        textBox1.AppendText("\r\n");
                    }
                }));
            }
            else
            {
                this.Invoke((EventHandler)(delegate
                {
                    NetLog(DateTime.Now.ToLongTimeString() + "   :   ");
                    NetLog("收到未知IP数据" + e.Client.clientEndPoint.ToString() + ":");
                    NetLog("\r\n");
                }));
            }
        }
        private void tcpAsyncServer_ErrorServer(object sender, ErrorServerEventArgs e)
        {
            this.Invoke((EventHandler)(delegate
            {
                NetLog(e.Error.Message + "\r\n");
            }));
        }

        private void Form2_FormClosing(object sender, FormClosingEventArgs e)
        {
            tcpAsyncServer.ErrorServerEvent -= new TCPErrorEvent(tcpAsyncServer_ErrorServer);
            tcpAsyncServer.ReceiveServerEvent -= new TCPReceiveEvent(tcpAsyncServer_ReceiveServerEvent);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (Form1.mcuClientContext!=null)
            {
                tabControl1.Enabled = false;
                timer1.Enabled = true;
                byte[] tx = CP1616_NoAddr_Packet.MakeCP1616_NoAddr_Packet(0x02);
                this.Invoke((EventHandler)(delegate
                {
                    textBox1.AppendText(DateTime.Now.ToLongTimeString() + "   :   ");
                    textBox1.AppendText("发送到" + Form1.mcuClientContext.clientEndPoint.ToString() + ":");
                    textBox1.AppendText(WFNetLib.StringFunc.StringsFunction.byteToHexStr(tx, " "));
                    textBox1.AppendText("\r\n");
                }));
                tcpAsyncServer.Send(Form1.mcuClientContext, tx);
            }
            else
            {
                MessageBox.Show("单片机采集板尚未接入，请稍后重试！！");
            }
        }
        IPAddress CheckIPInput(string str)
        {
            string[] ips = str.Split('.');
            int[] ipn = new int[4];
            if (ips.Length != 4)
            {
                return null;
            }
            for (int i = 0; i < 4; i++)
            {
                if (int.TryParse(ips[i], out ipn[i]) == false)
                {                    
                    return null;
                }
                if (ipn[i] > 255 || ipn[i] < 0)
                {
                    return null;
                }
            }
            byte[] ipb = new byte[4];
            for (int i = 0; i < 4; i++)
                ipb[i] = (byte)ipn[i];
            return new IPAddress(ipb);
        }
        private void button2_Click(object sender, EventArgs e)
        {
            if (Form1.mcuClientContext==null)
            {
                MessageBox.Show("单片机采集板尚未接入，请稍后重试！！");
                return;
            }
            IPAddress mcuIP = CheckIPInput(textBox2.Text);
            if (mcuIP==null)
            {
                MessageBox.Show("单片机板ip输入有误");
                return;
            }

            IPAddress mcuSN = CheckIPInput(textBox3.Text);
            if (mcuSN == null)
            {
                MessageBox.Show("单片机板子网掩码输入有误");
                return;
            }

            IPAddress mcuGW = CheckIPInput(textBox4.Text);
            if (mcuGW == null)
            {
                MessageBox.Show("单片机板默认网关输入有误");
                return;
            }

            IPAddress pcIP = CheckIPInput(textBox5.Text);
            if (pcIP == null)
            {
                MessageBox.Show("服务器端ip输入有误");
                return;
            }
            tabControl1.Enabled = false;
            timer1.Enabled = true;
            byte[] tx = new byte[28];
            for (int i = 0; i < 4; i++)
            {
                tx[i] = mcuIP.GetAddressBytes()[i];
                tx[4+i] = mcuSN.GetAddressBytes()[i];
                tx[8 + i] = mcuGW.GetAddressBytes()[i];
                tx[16 + i] = pcIP.GetAddressBytes()[i];
            }
            tx[20] = BytesOP.GetLowByte((ushort)numericUpDown2.Value);
            tx[21] = BytesOP.GetHighByte((ushort)numericUpDown2.Value);
            tx[22] = 0;
            tx[23] = 0;
            tx[24] = BytesOP.GetLowByte((ushort)numericUpDown1.Value);
            tx[25] = BytesOP.GetHighByte((ushort)numericUpDown1.Value);
            tx[26] = 0;
            tx[27] = 0;
            byte[] tx03 = CP1616_NoAddr_Packet.MakeCP1616_NoAddr_Packet(0x03,tx);
            this.Invoke((EventHandler)(delegate
            {
                textBox1.AppendText(DateTime.Now.ToLongTimeString() + "   :   ");
                textBox1.AppendText("发送到" + Form1.mcuClientContext.clientEndPoint.ToString() + ":");
                textBox1.AppendText(WFNetLib.StringFunc.StringsFunction.byteToHexStr(tx03, " "));
                textBox1.AppendText("\r\n");
            }));
            tcpAsyncServer.Send(Form1.mcuClientContext, tx03);
        }

        private void button3_Click(object sender, EventArgs e)
        {
            tabControl1.Enabled = false;
            timer1.Enabled = true;
            byte[] tx = new byte[2];
            tx[0] = BytesOP.GetHighByte((ushort)(numericUpDown3.Value*10));
            tx[1] = BytesOP.GetLowByte((ushort)(numericUpDown3.Value * 10));
            byte[] tx03 = CP1616_NoAddr_Packet.MakeCP1616_NoAddr_Packet(0x04, tx);
            this.Invoke((EventHandler)(delegate
            {
                textBox1.AppendText(DateTime.Now.ToLongTimeString() + "   :   ");
                textBox1.AppendText("发送到" + Form1.mcuClientContext.clientEndPoint.ToString() + ":");
                textBox1.AppendText(WFNetLib.StringFunc.StringsFunction.byteToHexStr(tx03, " "));
                textBox1.AppendText("\r\n");
            }));
            tcpAsyncServer.Send(Form1.mcuClientContext, tx03);
        }

        private void button4_Click(object sender, EventArgs e)
        {
            tabControl1.Enabled = false;
            timer1.Enabled = true;
            byte[] tx = new byte[6];
            if (rb1h.Checked)
                tx[0] = 0x01;
            else
                tx[0] = 0x00;
            if (rb2h.Checked)
                tx[1] = 0x01;
            else
                tx[1] = 0x00;
            if (rb3h.Checked)
                tx[2] = 0x01;
            else
                tx[2] = 0x00;
            if (rb4h.Checked)
                tx[3] = 0x01;
            else
                tx[3] = 0x00;
            if (rb5h.Checked)
                tx[4] = 0x01;
            else
                tx[4] = 0x00;
            if (rb6h.Checked)
                tx[5] = 0x01;
            else
                tx[5] = 0x00;

            byte[] tx03 = CP1616_NoAddr_Packet.MakeCP1616_NoAddr_Packet(0x05, tx);
            this.Invoke((EventHandler)(delegate
            {
                textBox1.AppendText(DateTime.Now.ToLongTimeString() + "   :   ");
                textBox1.AppendText("发送到" + Form1.mcuClientContext.clientEndPoint.ToString() + ":");
                textBox1.AppendText(WFNetLib.StringFunc.StringsFunction.byteToHexStr(tx03, " "));
                textBox1.AppendText("\r\n");
            }));
            tcpAsyncServer.Send(Form1.mcuClientContext, tx03);
        }

        private void button5_Click(object sender, EventArgs e)
        {
            tabControl1.Enabled = false;
            timer1.Enabled = true;
            byte[] tx03 = CP1616_NoAddr_Packet.MakeCP1616_NoAddr_Packet(0x06);
            this.Invoke((EventHandler)(delegate
            {
                textBox1.AppendText(DateTime.Now.ToLongTimeString() + "   :   ");
                textBox1.AppendText("发送到" + Form1.mcuClientContext.clientEndPoint.ToString() + ":");
                textBox1.AppendText(WFNetLib.StringFunc.StringsFunction.byteToHexStr(tx03, " "));
                textBox1.AppendText("\r\n");
            }));
            tcpAsyncServer.Send(Form1.mcuClientContext, tx03);
        }

        private void button6_Click(object sender, EventArgs e)
        {
            tabControl1.Enabled = false;
            timer1.Enabled = true;
            byte[] tx = new byte[2];
            if (rbp1on.Checked)
                tx[0] = 0x01;
            else
                tx[0] = 0x00;
            if (rbp2on.Checked)
                tx[1] = 0x01;
            else
                tx[1] = 0x00;            

            byte[] tx03 = CP1616_NoAddr_Packet.MakeCP1616_NoAddr_Packet(0x07, tx);
            this.Invoke((EventHandler)(delegate
            {
                textBox1.AppendText(DateTime.Now.ToLongTimeString() + "   :   ");
                textBox1.AppendText("发送到" + Form1.mcuClientContext.clientEndPoint.ToString() + ":");
                textBox1.AppendText(WFNetLib.StringFunc.StringsFunction.byteToHexStr(tx03, " "));
                textBox1.AppendText("\r\n");
            }));
            tcpAsyncServer.Send(Form1.mcuClientContext, tx03);
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            timer1.Enabled = false;
            MessageBox.Show("设定失败");
            tabControl1.Enabled = true;           
        }
        
    }
}
