import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.sql.ResultSet;
import java.sql.SQLException;
/**
 * @author Kosmischer
 */
public class ViewMyAccountInfo implements GetPanel{
    private JPanel mp;
    private JButton returnButton;
    private JLabel accountField;
    private JLabel nameField;
    private JLabel passwordField;
    private JLabel cityField;
    private JLabel privilegeField;
    private JLabel statusField;

    public ViewMyAccountInfo(int currentAccount) {
        String sql = "SELECT * FROM userinfo WHERE userAccount = " + currentAccount;
        try {
            ResultSet result = Main.statement.executeQuery(sql);
            result.next();
            accountField.setText(result.getObject("userAccount").toString());
            nameField.setText(result.getObject("userName").toString());
            passwordField.setText(result.getObject("userPassword").toString());
            cityField.setText(result.getObject("cityID").toString());
            privilegeField.setText(result.getObject("userPrivilege").toString());
            statusField.setText(result.getObject("userStatus").toString());

        } catch (SQLException ex) {
            throw new RuntimeException(ex);
        }
        returnButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    Main.switchClientPage(currentAccount);
                } catch (SQLException ex) {
                    throw new RuntimeException(ex);
                }
            }
        });
    }

    @Override
    public JPanel getPanel(){
        return mp;
    }
}
