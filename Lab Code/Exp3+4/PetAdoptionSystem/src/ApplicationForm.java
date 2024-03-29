import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Objects;

import static java.lang.Integer.parseInt;
import static jdk.dynalink.beans.StaticClass.forClass;
/**
 * @author Kosmischer
 */
public class ApplicationForm implements GetPanel{
    private JPanel mp;
    private JTextField petField;
    private JTextField nameField;
    private JTextField ageField;
    private JButton submitButton;
    private JButton returnButton;
    private JTextField cityField;
    private JTextField telField;
    private JTextField mailField;
    private JComboBox genderField;

    public ApplicationForm(int currentUserAccount) {

        returnButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    Main.switchClientPage(currentUserAccount);
                } catch (SQLException ex) {
                    throw new RuntimeException(ex);
                }
            }
        });
        submitButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                String sql = "SELECT COUNT(*) AS cnt FROM adoptapplication";
                try {
                    ResultSet result = Main.statement.executeQuery(sql);
                    result.next();
                    int applicationId = parseInt(result.getObject("cnt").toString()) + 1;
                    String adopterName = nameField.getText();
                    int petId = parseInt(petField.getText());
                    int adopterAge = parseInt(ageField.getText());
                    String adopterGender = switch (genderField.getSelectedIndex()) {
                        case 1 -> "Male";
                        case 2 -> "Female";
                        default -> "INVALID";
                    };
                    int adopterCity = parseInt(cityField.getText());
                    String adopterTEL = telField.getText();
                    String adopterMail = mailField.getText();
                    boolean isValid = true;
                    sql = "SELECT isAdopted FROM pet WHERE petID = " + petId;
                    result = Main.statement.executeQuery(sql);
                    if(!result.next()){
                        Main.switchErrorBox("Sorry, pet whose ID = " + petId + "does not exist.");
                        isValid = false;
                    }
                    if(Objects.equals(result.getObject("isAdopted").toString(), "true")) {
                        Main.switchErrorBox("Sorry, pet whose ID = " + petId + "has been adopted already.");
                        isValid = false;
                    }
                    if(Objects.equals(telField.getText(), "")){
                        isValid = false;
                    }
                    if(!isValid){
                        Main.switchErrorBox("Information filled is not valid");
                    }
                    else{
                        sql = "INSERT INTO adoptapplication VALUES (" +
                                applicationId +
                                ", " +
                                currentUserAccount +
                                ", " +
                                petId +
                                ", " +
                                null +
                                ", '" +
                                adopterName +
                                "', '" +
                                adopterGender +
                                "', " +
                                adopterAge +
                                ", " +
                                adopterCity +
                                ", '" +
                                adopterTEL +
                                "', '" +
                                adopterMail +
                                "', 0" +
                                ")";
                        System.out.println("SQL = " + sql);
                        Main.statement.execute(sql);
                        Main.switchErrorBox("Your application is submitted and waiting for administrator's approval");
                    }


                } catch (SQLException | NumberFormatException ex) {
                    ex.printStackTrace();
                    if(ex instanceof NumberFormatException){
                        Main.switchErrorBox("Number Format Error (Or Empty)");
                    }
                }
            }
        });
    }

    @Override
    public JPanel getPanel(){
        return mp;
    }

}
