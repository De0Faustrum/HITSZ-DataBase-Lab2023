import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.sql.SQLException;
/**
 * @author Kosmischer
 */
public class AdministratorPage implements GetPanel{
    private JPanel mp;
    private JButton freezeUnfreezeUserAccountButton;
    private JButton viewDatabaseButton;
    private JButton submitRevisitRecordButton;
    private JButton returnButton;
    private JButton examineAdoptApplicationButton;

    public AdministratorPage() {
        viewDatabaseButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                Main.switchDatabaseEntry();
            }
        });
        returnButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    Main.switchStartMenu();
                } catch (IOException ex) {
                    throw new RuntimeException(ex);
                }
            }
        });
        freezeUnfreezeUserAccountButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                Main.switchFreezeDefreeze();
            }
        });
        examineAdoptApplicationButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {

            }
        });
        examineAdoptApplicationButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    Main.switchExamineApplication();
                } catch (SQLException ex) {
                    throw new RuntimeException(ex);
                }
            }
        });
        submitRevisitRecordButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {
                    Main.switchSubmitRevistRecord();
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
