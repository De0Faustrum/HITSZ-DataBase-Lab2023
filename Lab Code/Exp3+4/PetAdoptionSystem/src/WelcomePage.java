import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
/**
 * @author Kosmischer
 */
public class WelcomePage extends JFrame implements GetPanel{
    private JPanel mp;
    private JLabel imageLabel = new JLabel(new ImageIcon("src/bkg.jpg"));
    private JButton clientLogInButton;
    private JButton administratorLogInButton;
    private JButton registerButton;
    private JLabel welcomeLabel1;
    private JLabel welcomeLabel2;

    public WelcomePage() throws IOException {
        mp.setLayout(null);
        mp.setOpaque(false);
        welcomeLabel1.setText("Welcome To");
        welcomeLabel2.setText("Pet Adopt System!");
        mp.add(welcomeLabel1);
        mp.add(welcomeLabel2);
        mp.add(imageLabel,-1);
        mp.add(clientLogInButton,1);
        administratorLogInButton.setBounds(1200, 150, 300, 100);
        clientLogInButton.setBounds(1200, 400, 300, 100);
        registerButton.setBounds(1200, 650, 300, 100);
        welcomeLabel1.setBounds(200,385,500,100);
        welcomeLabel2.setBounds(120,465,500,100);
        mp.add(administratorLogInButton,1);
        mp.add(registerButton,1);
        imageLabel.setBounds(0,0,1600,900);

        JLabel lbBg = new JLabel(new ImageIcon("scr/bkg.jpg"));
        lbBg.setBounds(0, 0, 1600, 900);
        this.getContentPane().add(lbBg);
        repaint();
        repaint();

        administratorLogInButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                Main.switchAdministratorPage();
            }
        });
        registerButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                Main.switchUserRegister();
            }
        });
        clientLogInButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                Main.switchLogIn();
            }
        });
    }

    @Override
    public JPanel getPanel(){
        return mp;
    }

    @Override
    public void paint(Graphics g){
        super.paint(g);
        g.drawImage(Main.BACKGROUND,100,100,null);
        System.out.println("REPAINT");
    }

}
