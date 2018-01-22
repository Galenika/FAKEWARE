using System;
using System.Windows;
using System.Runtime.InteropServices;
using System.Net;
using System.Net.Http;

namespace loader
{
    /// <summary>
    /// Interaction logic for Injector.xaml
    /// </summary>
    public partial class Injector : Window
    {
        public Injector() => InitializeComponent();

        [DllImport("injlib", CallingConvention = CallingConvention.Cdecl)]
        private extern static int inject();

        private void Inject(object sender, RoutedEventArgs e)
        {
            try
            {
                using (var client = new WebClient())
                {
#if DEBUG
#else
                    client.DownloadFile("https://luke.ams3.digitaloceanspaces.com/cheat.dll", "cheat.dll");
#endif
                    int result = inject();
                    if (result != 0)
                    {
                        MessageBox.Show("Injection failed.");
#if DEBUG
#else
                        Application.Current.Shutdown();
#endif
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
    }
}
