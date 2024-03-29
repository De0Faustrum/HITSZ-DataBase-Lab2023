import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Objects;

import static java.lang.Integer.parseInt;
/**
 * @author Kosmischer
 */
public class UserRegister implements GetPanel{
    private JPanel mp;
    private JTextField textField2;
    private JTextField textField3;
    private JTextField textField4;
    private JButton confirmButton;
    private JButton cancelButton;
    public UserRegister() {
        confirmButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                String sql = "Select Count(*) as cnt From userinfo";
                try {
                    ResultSet result = Main.statement.executeQuery(sql);
                    result.next();
                    int resultSize = parseInt(result.getObject("cnt").toString()) + 1;
                    if(!(Objects.equals(textField2.getText(), "")||Objects.equals(textField3.getText(), "")||Objects.equals(textField4.getText(), ""))){
                        sql = "INSERT INTO userinfo VALUES ("
                                + resultSize + ", "
                                + parseInt(textField4.getText()) + ", '"
                                + textField2.getText() + "', '"
                                + textField3.getText() + "', 'Client', 'Normal')";
                        System.out.println("The Sql is: " + sql);
                        Main.statement.execute(sql);
                        Main.switchErrorBox("Congratulations! You have completed your register, and your account is " + resultSize);
                        Main.switchStartMenu();
                    }
                    else {
                        Main.switchErrorBox("Please Fill All The Form.");
                        System.out.println("Please Fill All The Form.");
                    }
                } catch (SQLException ex) {
                    throw new RuntimeException(ex);
                } catch (IOException ex) {
                    throw new RuntimeException(ex);
                }

            }
        });
        cancelButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    Main.switchStartMenu();
                } catch (IOException ex) {
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
