import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Objects;

import static java.lang.Integer.parseInt;
/**
 * @author Kosmischer
 */
public class ExamineApplication implements GetPanel{
    private JPanel mp;
    private JTextField textField1;
    private JButton findRecordButton;
    private JButton acceptButton;
    private JButton returnButton;
    private JLabel accountLabel;
    private JLabel nameLabel;
    private JLabel petLabel;
    private JLabel petNameLabel;
    private JLabel breedLabel;
    private JLabel genderLabel;
    private JLabel ageLabel;
    private JLabel cityLabel;
    private JLabel telLabel;
    private JLabel veryLabel;
    private int applicationID = 0;
    private int petID = 0;
    private String sql1, sql2, sql3, sql4, sql5;

    public ExamineApplication() {
        findRecordButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                applicationID = parseInt(textField1.getText());
                sql1 = "SELECT * FROM adoptapplication WHERE applicationID = " + applicationID;
                try {
                    ResultSet result = Main.statement.executeQuery(sql1);
                    if(!result.next()){
                        Main.switchErrorBox("Application whose ID = " + applicationID + " does not exist.");
                    }
                    else {
                        sql2 =  "SELECT userinfo.userAccount AS Account," +
                                "adoptapplication.adopterName AS AName," +
                                "adoptapplication.petID AS PetID," +
                                "adoptapplication.adopterGender AS Gender," +
                                "adoptapplication.adopterAge AS Age," +
                                "adoptapplication.adopterTEL AS Tel," +
                                "adoptapplication.isApproved AS Verification," +
                                "pet.petName AS PName," +
                                "breed.breedName AS BName," +
                                "city.cityName AS CName" +
                                " FROM userinfo, adoptapplication, pet, breed, city" +
                                " WHERE adoptapplication.applicationID = " + applicationID +
                                " And userinfo.userAccount = adoptapplication.userAccount" +
                                " And adoptapplication.petID = pet.petID" +
                                " And pet.breedID = breed.breedID" +
                                " And adoptApplication.adopterCityID = city.cityID";
                    }
                    result = Main.statement.executeQuery(sql2);
                    result.next();
                    petID = parseInt(result.getObject("PetID").toString());
                    accountLabel.setText(result.getObject("Account").toString());
                    nameLabel.setText(result.getObject("AName").toString());
                    petLabel.setText(result.getObject("PetID").toString());
                    petNameLabel.setText(result.getObject("PName").toString());
                    breedLabel.setText(result.getObject("BName").toString());
                    genderLabel.setText(result.getObject("Gender").toString());
                    ageLabel.setText(result.getObject("Age").toString());
                    cityLabel.setText(result.getObject("CName").toString());
                    telLabel.setText(result.getObject("Tel").toString());
                    veryLabel.setText(result.getObject("Verification").toString());
                } catch (SQLException ex) {
                    throw new RuntimeException(ex);
                }
            }
        });
        acceptButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if(Objects.equals(accountLabel.getText(), "<DEFAULT>")){
                    Main.switchErrorBox("Please select an application before operation.");
                }
                else if(Objects.equals(veryLabel.getText(), "true")) {
                    Main.switchErrorBox("This application has been approved already.");
                }
                else {
                    try {
                        sql3 =  "SELECT COUNT(*) AS cnt FROM adoptinfo";
                        ResultSet result = Main.statement.executeQuery(sql3);
                        result.next();
                        int adoptID = parseInt(result.getObject("cnt").toString()) + 1;
                        sql4 =  "INSERT INTO adoptinfo VALUES (" + adoptID + ", " + applicationID + ", " + petID + ", '2023-10-1', 0)";
                        sql5 =  "UPDATE adoptapplication SET adoptID = " + adoptID +
                                " WHERE applicationID = " + applicationID;
                        System.out.println("SQL: " + sql4);
                        System.out.println("SQL: " + sql5 );
                        Main.statement.execute(sql4);
                        Main.statement.execute(sql5);
                        sql5 =  "UPDATE adoptapplication SET isApproved = true" +
                                " WHERE applicationID = " + applicationID;
                        System.out.println("SQL: " + sql5 );
                        Main.statement.execute(sql5);
                        findRecordButton.doClick();
                    } catch (SQLException ex) {
                        throw new RuntimeException(ex);
                    }
                }
            }
        });
        returnButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                Main.switchAdministratorPage();
            }
        });
    }

    @Override
    public JPanel getPanel() {
        return mp;
    }
}
