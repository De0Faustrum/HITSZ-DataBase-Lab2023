import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.sql.*;

import static java.lang.Integer.parseInt;
/**
 * @author Kosmischer
 */
public class DatabaseTableView implements GetPanel{
    private JButton returnButton;
    private JTable table1;
    private JPanel mp;
    private JScrollPane table;
    String[] columnName;
    private String[][] tableData;

    public DatabaseTableView(String tableName) throws SQLException {
        switch (tableName){
            case "Pet": initPet(); break;
            case "UserInfo": initUserInfo(); break;
            case "AdoptApplication": initAdoptApplication(); break;
            case "AdoptInfo": initAdoptInfo(); break;
            case "Breed": initBreed(); break;
            case "City": initCity(); break;
            case "Province": initProvince(); break;
            case "RevisitRecord": initRevisitRecord(); break;
            case "Adopter_Pet": initAdopterPet(); break;
            case "User_City_Province": initUserCityProvince(); break;
            default:break;
        }

        DefaultTableModel tableModel = new DefaultTableModel(tableData, columnName) {
            @Override
            public boolean isCellEditable(int row, int col) {
                return false;
            }
        };

        Font font = new Font("Droid Sans Mono", Font.PLAIN, 16);
        table1.setFont(font);
        table1.setRowHeight(24);
        table1.setAlignmentX(Component.CENTER_ALIGNMENT);
        table.setFont(font);
        table1.setModel(tableModel);
        table.setViewportView(table1);

        returnButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                Main.switchDatabaseEntry();
            }
        });
    }

    public void initPet() throws SQLException {
        System.out.println("INIT TABLE PET");
        columnName = new String[]{"petID", "petName", "birthday", "age", "gender", "breedID", "adoptID", "colour", "petCharacter", "isAdopted", "isApplied"};
        String sql = "Select COUNT(*) As cnt From pet";
        ResultSet result = Main.statement.executeQuery(sql);
        result.next();
        int resultSize = parseInt(result.getObject("cnt").toString());
        tableData = new String[resultSize][11];
        sql = "Select * From pet";
        result = Main.statement.executeQuery(sql);
        int i = 0;
        while (result.next()) {
            for(int j=0; j<11; j++) {
                try {
                    tableData[i][j] = result.getObject(columnName[j]).toString();
                } catch (NullPointerException e) {
                    tableData[i][j] = "null";
                }
            }
            i++;
        }
        result.close();
    }

    public void initUserInfo() throws SQLException {
        System.out.println("INIT TABLE USER INFO");
        columnName = new String[]{"userAccount", "cityID", "userName", "userPassword", "userPrivilege", "userStatus"};
        String sql = "Select COUNT(*) As cnt From userinfo";
        ResultSet result = Main.statement.executeQuery(sql);
        result.next();
        int resultSize = parseInt(result.getObject("cnt").toString());
        tableData = new String[resultSize][6];
        sql = "Select * From userinfo";
        result = Main.statement.executeQuery(sql);
        int i = 0;
        while (result.next()){
            for(int j=0; j<6; j++){
                try {
                    tableData[i][j] = result.getObject(columnName[j]).toString();
                }
                catch (NullPointerException e){
                    tableData[i][j] = "null";
                }
            }
            i++;
        }
        result.close();
    }

    public void initAdoptApplication() throws SQLException {
        System.out.println("INIT TABLE ADOPT APPLICATION");
        columnName = new String[]{"applicationID","userAccount","petID","adoptID","adopterName","adopterGender","adopterAge","adopterCityID","adopterTEL","adopterMail","isApproved"};
        String sql = "Select COUNT(*) As cnt From adoptapplication";
        ResultSet result = Main.statement.executeQuery(sql);
        result.next();
        int resultSize = parseInt(result.getObject("cnt").toString());
        tableData = new String[resultSize][11];
        sql = "Select * From adoptapplication";
        result = Main.statement.executeQuery(sql);
        int i = 0;
        while (result.next()){
            for(int j=0; j<11; j++){
                try {
                    tableData[i][j] = result.getObject(columnName[j]).toString();
                }
                catch (NullPointerException e){
                    tableData[i][j] = "null";
                }
            }
            i++;
        }
        result.close();
    }

    public void initAdoptInfo() throws SQLException {
        System.out.println("INIT TABLE ADOPT INFO");
        columnName = new String[]{"adoptID", "applicationID", "petID", "adoptDate", "adoptTime"};
        String sql = "Select COUNT(*) As cnt From adoptinfo";
        ResultSet result = Main.statement.executeQuery(sql);
        result.next();
        int resultSize = parseInt(result.getObject("cnt").toString());
        tableData = new String[resultSize][5];
        sql = "Select * From adoptinfo";
        result = Main.statement.executeQuery(sql);
        int i = 0;
        while (result.next()){
            for(int j=0; j<5; j++){
                try {
                    tableData[i][j] = result.getObject(columnName[j]).toString();
                }
                catch (NullPointerException e){
                    tableData[i][j] = "null";
                }
            }
            i++;
        }
        result.close();
    }

    public void initBreed() throws SQLException {
        System.out.println("INIT TABLE BREED");
        columnName = new String[]{"breedID", "breedName"};
        String sql = "Select COUNT(*) As cnt From breed";
        ResultSet result = Main.statement.executeQuery(sql);
        result.next();
        int resultSize = parseInt(result.getObject("cnt").toString());
        tableData = new String[resultSize][2];
        sql = "Select * From breed";
        result = Main.statement.executeQuery(sql);
        int i = 0;
        while (result.next()){
            for(int j=0; j<2; j++){
                try {
                    tableData[i][j] = result.getObject(columnName[j]).toString();
                }
                catch (NullPointerException e){
                    tableData[i][j] = "null";
                }
            }
            i++;
        }
        result.close();
    }

    public void initCity() throws SQLException {
        System.out.println("INIT TABLE CITY");
        columnName = new String[]{"cityID", "cityName", "provinceID"};
        String sql = "Select COUNT(*) As cnt From city";
        ResultSet result = Main.statement.executeQuery(sql);
        result.next();
        int resultSize = parseInt(result.getObject("cnt").toString());
        tableData = new String[resultSize][3];
        sql = "Select * From city";
        result = Main.statement.executeQuery(sql);
        int i = 0;
        while (result.next()){
            for(int j=0; j<3; j++){
                try {
                    tableData[i][j] = result.getObject(columnName[j]).toString();
                }
                catch (NullPointerException e){
                    tableData[i][j] = "null";
                }
            }
            i++;
        }
        result.close();
    }

    public void initProvince() throws SQLException {
        System.out.println("INIT TABLE PROVINCE");
        columnName = new String[]{"provinceID", "provinceName"};
        String sql = "Select COUNT(*) As cnt From province";
        ResultSet result = Main.statement.executeQuery(sql);
        result.next();
        int resultSize = parseInt(result.getObject("cnt").toString());
        tableData = new String[resultSize][2];
        sql = "Select * From province";
        result = Main.statement.executeQuery(sql);
        int i = 0;
        while (result.next()){
            for(int j=0; j<2; j++){
                try {
                    tableData[i][j] = result.getObject(columnName[j]).toString();
                }
                catch (NullPointerException e){
                    tableData[i][j] = "null";
                }
            }
            i++;
        }
        result.close();
    }

    public void initRevisitRecord() throws SQLException {
        System.out.println("INIT TABLE REVISIT RECORD");
        columnName = new String[]{"revisitID", "adoptID", "revisitcomment", "revisitDate", "petID"};
        String sql = "Select COUNT(*) As cnt From revisitrecord";
        ResultSet result = Main.statement.executeQuery(sql);
        result.next();
        int resultSize = parseInt(result.getObject("cnt").toString());
        tableData = new String[resultSize][5];
        sql = "Select * From revisitrecord";
        result = Main.statement.executeQuery(sql);
        int i = 0;
        while (result.next()){
            for(int j=0; j<5; j++){
                try {
                    tableData[i][j] = result.getObject(columnName[j]).toString();
                }
                catch (NullPointerException e){
                    tableData[i][j] = "null";
                }
            }
            i++;
        }
        result.close();
    }

    public void initAdopterPet() throws SQLException {
        System.out.println("INIT VIEW ADOPTER_PET");
        columnName = new String[]{"adopterName", "petName"};
        String sql = "Select COUNT(*) As cnt From adopter_pet";
        ResultSet result = Main.statement.executeQuery(sql);
        result.next();
        int resultSize = parseInt(result.getObject("cnt").toString());
        tableData = new String[resultSize][2];
        sql = "Select * From adopter_pet";
        result = Main.statement.executeQuery(sql);
        int i = 0;
        while (result.next()){
            for(int j=0; j<2; j++){
                try {
                    tableData[i][j] = result.getObject(columnName[j]).toString();
                }
                catch (NullPointerException e){
                    tableData[i][j] = "null";
                }
            }
            i++;
        }
        result.close();
    }

    public void initUserCityProvince() throws SQLException {
        System.out.println("INIT TABLE USER_CITY_PROVINCE");
        columnName = new String[]{"userName","city","province"};
        String sql = "Select COUNT(*) As cnt From user_city_province";
        ResultSet result = Main.statement.executeQuery(sql);
        result.next();
        int resultSize = parseInt(result.getObject("cnt").toString());
        tableData = new String[resultSize][3];
        sql = "Select * From user_city_province";
        result = Main.statement.executeQuery(sql);
        int i = 0;
        while (result.next()){
            for(int j=0; j<3; j++){
                try {
                    tableData[i][j] = result.getObject(columnName[j]).toString();
                }
                catch (NullPointerException e){
                    tableData[i][j] = "null";
                }
            }
            i++;
        }
        result.close();
    }


    @Override
    public JPanel getPanel(){
        return mp;
    }

}
