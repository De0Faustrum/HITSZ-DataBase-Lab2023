import javax.imageio.ImageIO;
import javax.imageio.stream.ImageInputStream;
import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.*;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Objects;

/**
 * @author Kosmischer
 */
public class Main extends JPanel{
    public static BufferedImage BACKGROUND;
    public static final Dimension DIMENSION_OF_FRAME = new Dimension(1600, 900);
    public static final CardLayout CARD_LAYOUT = new CardLayout(0, 0);
    public static final JPanel CARD_PANEL = new JPanel(CARD_LAYOUT);
    public static final JFrame MAIN_FRAME = new JFrame();
    public static final Dimension SCREEN_SIZE = Toolkit.getDefaultToolkit().getScreenSize();
    public static Main GLOBAL_INSTANCE;
    public static Connection connection;
    public static Statement statement;

    public Main() throws ClassNotFoundException, SQLException, IOException {
        super();
        Class.forName("com.mysql.cj.jdbc.Driver");
        String url="jdbc:mysql://localhost:3306/petadoptsystem?useUnicode=true&characterEncoding=utf8&useSSL=true";
        String username="root";
        String password="**************";
        connection = DriverManager.getConnection(url,username,password);
        statement = connection.createStatement();
        switchStartMenu();
    }

    public static Main getGlobalInstance() throws IOException {
        return GLOBAL_INSTANCE;
    }

    public static void main(String[] args) throws SQLException, ClassNotFoundException, IOException {
        MAIN_FRAME.setIconImage(ImageIO.read(new File("src/bkg.jpg")));
        BACKGROUND = ImageIO.read(new FileInputStream("src/bkg.jpg"));
        WelcomePage welcomePage = new WelcomePage(){
            public void paintComponent(Graphics g) {
                Image img = Toolkit.getDefaultToolkit().getImage(
                        JFrame.class.getResource("src/bkg.jpg"));
                g.drawImage(img, 0, 0, 1600, 900, this);
            }
        };

        MAIN_FRAME.setSize(DIMENSION_OF_FRAME);
        MAIN_FRAME.setResizable(true);
        MAIN_FRAME.setBounds(((int) SCREEN_SIZE.getWidth() - 1600) / 2, ((int) SCREEN_SIZE.getHeight() - 900)/2, 1600, 900);
        MAIN_FRAME.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        CARD_PANEL.add(welcomePage.getPanel());
        CARD_LAYOUT.next(CARD_PANEL);
        MAIN_FRAME.add(CARD_PANEL);
        MAIN_FRAME.setTitle("Pet Adopt System");
        MAIN_FRAME.setVisible(true);
        GLOBAL_INSTANCE = new Main();
    }

    public static void switchStartMenu() throws IOException {
        WelcomePage welcomePage = new WelcomePage();
        MAIN_FRAME.setTitle("Pet Adopt System");
        CARD_PANEL.add(welcomePage.getPanel());
        CARD_LAYOUT.next(CARD_PANEL);
        MAIN_FRAME.add(CARD_PANEL);
        MAIN_FRAME.setVisible(true);
    }

    public static void switchAdministratorPage(){
        AdministratorPage administratorPage = new AdministratorPage();
        CARD_PANEL.add(administratorPage.getPanel());
        CARD_LAYOUT.next(CARD_PANEL);
        MAIN_FRAME.add(CARD_PANEL);
        MAIN_FRAME.setVisible(true);
    }

    public static void switchDatabaseEntry(){
        DatabaseEntry databaseEntry = new DatabaseEntry();
        CARD_PANEL.add(databaseEntry.getPanel());
        CARD_LAYOUT.next(CARD_PANEL);
        MAIN_FRAME.add(CARD_PANEL);
        MAIN_FRAME.setVisible(true);
    }

    public static void switchDatabaseTableView(String tableName) throws SQLException, ClassNotFoundException {
        DatabaseTableView databaseTableView = new DatabaseTableView(tableName);
        CARD_PANEL.add(databaseTableView.getPanel());
        CARD_LAYOUT.next(CARD_PANEL);
        MAIN_FRAME.add(CARD_PANEL);
        MAIN_FRAME.setVisible(true);
    }


    public static void switchUserRegister() {
        UserRegister userRegister = new UserRegister();
        CARD_PANEL.add(userRegister.getPanel());
        CARD_LAYOUT.next(CARD_PANEL);
        MAIN_FRAME.add(CARD_PANEL);
        MAIN_FRAME.setVisible(true);
    }

    public static void switchLogIn() {
        LogIn logIn = new LogIn();
        CARD_PANEL.add(logIn.getPanel());
        CARD_LAYOUT.next(CARD_PANEL);
        MAIN_FRAME.add(CARD_PANEL);
        MAIN_FRAME.setVisible(true);
    }

    public static void switchClientPage(int currentUserAccount) throws SQLException {
        ClientPage clientPage = new ClientPage(currentUserAccount);
        CARD_PANEL.add(clientPage.getPanel());
        CARD_LAYOUT.next(CARD_PANEL);
        MAIN_FRAME.add(CARD_PANEL);
        MAIN_FRAME.setVisible(true);
    }

    public static void switchFreezeDefreeze() {
        FreezeDefreeze freezeDefreeze = new FreezeDefreeze();
        CARD_PANEL.add(freezeDefreeze.getPanel());
        CARD_LAYOUT.next(CARD_PANEL);
        MAIN_FRAME.add(CARD_PANEL);
        MAIN_FRAME.setVisible(true);
    }

    public static void switchApplicationForm(int userAccount) {
        ApplicationForm applicationForm = new ApplicationForm(userAccount);
        CARD_PANEL.add(applicationForm.getPanel());
        CARD_LAYOUT.next(CARD_PANEL);
        MAIN_FRAME.add(CARD_PANEL);
        MAIN_FRAME.setVisible(true);
    }

    public static void switchCheckMyApplication(int userAccount) throws SQLException {
        CheckMyApplication checkMyApplication = new CheckMyApplication(userAccount);
        CARD_PANEL.add(checkMyApplication.getPanel());
        CARD_LAYOUT.next(CARD_PANEL);
        MAIN_FRAME.add(CARD_PANEL);
        MAIN_FRAME.setVisible(true);
    }

    public static void switchViewMyAccountInfo(int userAccount) throws SQLException {
        ViewMyAccountInfo viewMyAccountInfo = new ViewMyAccountInfo(userAccount);
        CARD_PANEL.add(viewMyAccountInfo.getPanel());
        CARD_LAYOUT.next(CARD_PANEL);
        MAIN_FRAME.add(CARD_PANEL);
        MAIN_FRAME.setVisible(true);
    }

    public static void switchExamineApplication() throws SQLException {
        ExamineApplication examineApplication = new ExamineApplication();
        CARD_PANEL.add(examineApplication.getPanel());
        CARD_LAYOUT.next(CARD_PANEL);
        MAIN_FRAME.add(CARD_PANEL);
        MAIN_FRAME.setVisible(true);
    }

    public static void switchSubmitRevistRecord() throws SQLException {
        SubmitRevisitRecord submitRevisitRecord = new SubmitRevisitRecord();
        CARD_PANEL.add(submitRevisitRecord.getPanel());
        CARD_LAYOUT.next(CARD_PANEL);
        MAIN_FRAME.add(CARD_PANEL);
        MAIN_FRAME.setVisible(true);
    }

    public static void switchViewPets(int userAccount) throws SQLException {
        ViewPets viewPets = new ViewPets(userAccount);
        CARD_PANEL.add(viewPets.getPanel());
        CARD_LAYOUT.next(CARD_PANEL);
        MAIN_FRAME.add(CARD_PANEL);
        MAIN_FRAME.setVisible(true);
    }

    public static void switchErrorBox(String err) {
        ErrorBox errorBox = new ErrorBox(err);
        errorBox.setTitle("Password Error");
        errorBox.setBounds(((int) SCREEN_SIZE.getWidth() - 400) / 2, ((int) SCREEN_SIZE.getHeight() - 270)/2, 400, 270);
        errorBox.setResizable(false);
        errorBox.pack();
        errorBox.setVisible(true);
    }

    @Override
    public void paint(Graphics g){
        super.paint(g);
        g.drawImage(Main.BACKGROUND,100,100,null);
        System.out.println("REPAINT");
    }
}