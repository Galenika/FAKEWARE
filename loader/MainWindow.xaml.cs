using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace loader
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow() => InitializeComponent();

        private void Login(object sender, RoutedEventArgs e)
        {
            var InjectWindow = new Injector();
            InjectWindow.Show();
            Close();
            return;

            /*string user = username.Text;
            string pass = password.Password;
            if (user == "luke" && pass == "secret")
            {
                var InjectWindow = new Injector();
                InjectWindow.Show();
                Close();
            } else
            {
                MessageBox.Show("Account details incorrect.");
            }*/
        }

        private void Username_KeyDown(object sender, KeyEventArgs e)
        {
            if(e.Key == Key.Enter)
            {
                Login(this, e: new RoutedEventArgs());
            }
        }

        private void Password_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                Login(this, e: new RoutedEventArgs());
            }
        }
    }
}
