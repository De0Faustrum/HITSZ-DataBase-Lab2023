import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Objects;
/**
 * @author Kosmischer
 */
public class LogIn implements GetPanel{
    private JPanel mp;
    private JButton logInButton;
    private JButton returnButton;
    private JTextField accountField;
    private JTextField passwordField;
    private ResultSet result;
    public LogIn() {

        logInButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                boolean isFound = false;
                if(Objects.equals(accountField.getText(), "") && Objects.equals(passwordField.getText(), "")){
                    Main.switchErrorBox("Account Or Password Is Empty!");
                }
                else {
                    String sql = "Select * From userinfo";
                    try {
                        result = Main.statement.executeQuery(sql);
                        while(result.next()){
                            if(Objects.equals(result.getObject("userAccount").toString(), accountField.getText())){
                                isFound = true;
                                System.out.println("Password: "+result.getObject("userPassword"));
                                if(Objects.equals(result.getObject("userPassword").toString(), passwordField.getText())){
                                    Main.switchClientPage((Integer) result.getObject("userAccount"));
                                }
                                else{
                                    Main.switchErrorBox("Password Error!");
                                }
                            }
                        }
                        if(!isFound){
                            Main.switchErrorBox("Account Not Found");
                        }

                    } catch (SQLException ex) {
                        ;
                    }
                }

            }
        });
        returnButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    Main.switchStartMenu();
                } catch (IOException ex) {
                    ;
                }
            }
        });
    }

    @Override
    public JPanel getPanel(){
        return mp;
    }
}
