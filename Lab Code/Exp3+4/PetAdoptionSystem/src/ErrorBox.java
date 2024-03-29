import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
/**
 * @author Kosmischer
 */
public class ErrorBox extends JDialog {
    private JPanel contentPane;
    private JButton buttonOK;
    private JLabel notice;

    public ErrorBox(String err) {
        notice.setText(err);
        setContentPane(contentPane);
        setModal(true);
        getRootPane().setDefaultButton(buttonOK);

        buttonOK.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                onOK();
            }
        });
    }

    private void onOK() {
        // 在此处添加您的代码
        dispose();
    }

    public static void main(String[] args) {
        ErrorBox dialog = new ErrorBox("Error");
        dialog.pack();
        dialog.setVisible(true);
        System.exit(0);
    }
}
