/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package redblue.visualization;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.util.Random;
import javax.swing.*;

/**
 *
 * @author Kyle
 */
public class RedBlueVisualization {

    private static class Grid {

        public int gridSize;
        public int[][] grid;

        public Grid(int gridSize, double density) {
            this.gridSize = gridSize;
            grid = new int[gridSize][gridSize];
            Random r = new Random();
            for (int i = 0; i < gridSize; i++) {
                for (int j = 0; j < gridSize; j++) {
                    if (r.nextDouble() < density) {
                        if (r.nextBoolean()) {
                            grid[i][j] = 1;
                        } else {
                            grid[i][j] = 2;
                        }
                    } else {
                        grid[i][j] = 0;
                    }
                }
            }
        }

        public Grid(int gridSize, String inputGrid) {
            this.gridSize = gridSize;
            grid = new int[gridSize][gridSize];
            for (int i = 0; i < gridSize; i++) {
                for (int j = 0; j < gridSize; j++) {
                    grid[i][j] = Integer.parseInt(Character.toString(inputGrid.charAt(i * gridSize + j)));
                }
            }
        }

        @Override
        public String toString() {
            String returnValue = "";
            for (int i = 0; i < gridSize; i++) {
                String line = "";
                for (int j = 0; j < gridSize; j++) {
                    line += grid[i][j] + " ";
                }
                returnValue += line.trim() + "\n";
            }
            return returnValue;
        }

        public void next() {
            int[][] output = new int[gridSize][gridSize];

            //Red
            for (int i = 0; i < gridSize; i++) {
                for (int j = 0; j < gridSize; j++) {
                    if (grid[i][j] == 1) {
                        if (grid[i][(j + 1) % gridSize] == 0) {
                            output[i][j] = 0;
                            output[i][(j + 1) % gridSize] = 1;
                        } else {
                            output[i][j] = 1;
                        }
                    }
                }
            }

            //Blue
            for (int i = 0; i < gridSize; i++) {
                for (int j = 0; j < gridSize; j++) {
                    if (grid[i][j] == 2) {
                        if ((grid[(i + 1) % gridSize][j] == 0 || grid[(i + 1) % gridSize][j] == 1) && output[(i + 1) % gridSize][j] == 0) {
                            output[i][j] = 0;
                            output[(i + 1) % gridSize][j] = 2;
                        } else {
                            output[i][j] = 2;
                        }
                    }
                }
            }

            grid = output;
        }
    }
    private static JPanel tiles;
    private static JPanel buttons;
    private static JPanel options;
    private static JPanel saveLoad;
    private static int looping = 0;
    private static int gridSize = 32;
    private static int tileSize = 16;
    private static double density = 0.25;
    private static double threshold = 0.5;
    private static double fps = 20;
    private static Grid grid;

    public static void main(String[] args) {
        grid = new Grid(gridSize, density);
        showWindow();
    }

    private static void showWindow() {
        GridBagConstraints c;
        final JFrame f = new JFrame("Red/Blue Visualization");
        f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        f.setLayout(new GridBagLayout());

        try {
            javax.swing.UIManager.setLookAndFeel(javax.swing.UIManager.getSystemLookAndFeelClassName());
        } catch (Exception e) {
        }

        JPanel p = new JPanel(new GridBagLayout(), true);

        //Control Buttons
        buttons = new JPanel(new GridBagLayout(), true);
        c = new GridBagConstraints();
        c.gridy = 0;
        c.fill = GridBagConstraints.BOTH;
        p.add(buttons, c);

        JButton jb = new JButton("Start");
        c = new GridBagConstraints();
        c.gridx = 1;
        jb.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                new Thread(new beginAnimation()).start();
            }
        });
        buttons.add(jb, c);

        jb = new JButton("Stop");
        c = new GridBagConstraints();
        c.gridx = 2;
        jb.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                looping = 0;
            }
        });
        buttons.add(jb, c);

        jb = new JButton("Step");
        c = new GridBagConstraints();
        c.gridx = 3;
        c.insets = new Insets(0, 30, 0, 0);
        jb.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                grid.next();
                updateTiles();
                incItr();
            }
        });
        buttons.add(jb, c);

        JLabel l = new JLabel("Itr: 0");
        c = new GridBagConstraints();
        c.gridx = 4;
        c.weightx = 1;
        c.anchor = GridBagConstraints.LINE_START;
        c.insets = new Insets(0, 10, 0, 10);
        buttons.add(l, c);

        jb = new JButton("Options");
        c = new GridBagConstraints();
        c.gridx = 5;
        c.weightx = 1;
        c.anchor = GridBagConstraints.LINE_END;
        jb.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                options.setVisible(!options.isVisible());
                saveLoad.setVisible(!saveLoad.isVisible());
                f.pack();
            }
        });
        buttons.add(jb, c);

        //Options Buttons
        options = new JPanel(new GridBagLayout(), true);
        c = new GridBagConstraints();
        c.gridy = 1;
        c.ipady = 10;
        c.fill = GridBagConstraints.BOTH;
        options.setVisible(false);
        p.add(options, c);

        l = new JLabel("Gridsize: ");
        c = new GridBagConstraints();
        c.gridx = 0;
        c.insets = new Insets(0, 10, 0, 0);
        options.add(l, c);

        JTextField t = new JTextField("" + gridSize, 3);
        c = new GridBagConstraints();
        c.gridx = 1;
        options.add(t, c);

        l = new JLabel("Tilesize: ");
        c = new GridBagConstraints();
        c.gridx = 2;
        c.insets = new Insets(0, 10, 0, 0);
        options.add(l, c);

        t = new JTextField("" + tileSize, 3);
        c = new GridBagConstraints();
        c.gridx = 3;
        options.add(t, c);

        l = new JLabel("Density: ");
        c = new GridBagConstraints();
        c.gridx = 4;
        c.insets = new Insets(0, 10, 0, 0);
        options.add(l, c);

        t = new JTextField("" + density, 4);
        c = new GridBagConstraints();
        c.gridx = 5;
        options.add(t, c);

        l = new JLabel("Threshold: ");
        c = new GridBagConstraints();
        c.gridx = 6;
        c.insets = new Insets(0, 10, 0, 0);
        options.add(l, c);

        t = new JTextField("" + threshold, 4);
        c = new GridBagConstraints();
        c.gridx = 7;
        options.add(t, c);

        l = new JLabel("FPS: ");
        c = new GridBagConstraints();
        c.insets = new Insets(0, 10, 0, 0);
        c.gridx = 8;
        options.add(l, c);

        t = new JTextField("" + fps, 3);
        c = new GridBagConstraints();
        c.gridx = 9;
        c.weightx = 1;
        c.anchor = GridBagConstraints.LINE_START;
        options.add(t, c);

        jb = new JButton("Set");
        jb.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                gridSize = Integer.parseInt(((JTextField) options.getComponent(1)).getText());
                tileSize = Integer.parseInt(((JTextField) options.getComponent(3)).getText());
                density = Double.parseDouble(((JTextField) options.getComponent(5)).getText());
                threshold = Double.parseDouble(((JTextField) options.getComponent(7)).getText());
                fps = Double.parseDouble(((JTextField) options.getComponent(9)).getText());
                grid = new Grid(gridSize, density);
                f.dispose();
                showWindow();
            }
        });
        c = new GridBagConstraints();
        c.gridx = 10;
        options.add(jb, c);

        saveLoad = new JPanel(new GridBagLayout(), true);
        c = new GridBagConstraints();
        c.gridy = 2;
        c.ipady = 10;
        c.fill = GridBagConstraints.BOTH;
        saveLoad.setVisible(false);
        p.add(saveLoad, c);

        jb = new JButton("Save");
        jb.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent ae) {
                try {
                    FileWriter file = new FileWriter("grid" + gridSize + "x" + gridSize + ".txt");
                    BufferedWriter bw = new BufferedWriter(file);
                    bw.write(convert(grid.toString()));
                    bw.close();
                } catch (Exception e) {
                    System.out.println("Error: " + e.getMessage());
                }
            }
        });
        c = new GridBagConstraints();
        c.gridx = 0;
        saveLoad.add(jb, c);

        jb = new JButton("Load");
        jb.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent ae) {
                try {
                    String gridInput = "";
                    FileReader file = new FileReader("grid" + gridSize + "x" + gridSize + ".txt");
                    BufferedReader br = new BufferedReader(file);
                    String line;
                    while ((line = br.readLine()) != null) {
                        line = convert(line);
                        line = line.replace(" ", "");
                        gridSize = line.length();
                        gridInput += line;
                    }

                    grid = new Grid(gridSize, gridInput);
                    f.dispose();
                    showWindow();
                } catch (Exception e) {
                    System.out.println("Error: " + e.getMessage());
                }
            }
        });
        c = new GridBagConstraints();
        c.gridx = 1;
        c.weightx = 1;
        c.anchor = GridBagConstraints.LINE_START;
        saveLoad.add(jb, c);

        //Colored Tiles
        tiles = new JPanel(new GridBagLayout(), true);
        c = new GridBagConstraints();
        c.gridy = 3;
        p.add(tiles, c);

        for (int i = 0; i < gridSize; i++) {
            for (int j = 0; j < gridSize; j++) {
                JPanel pp = new JPanel();
                c = new GridBagConstraints();
                c.gridx = j;
                c.gridy = i;
                tiles.add(pp, c);
            }
        }

        updateTiles();

        f.add(p);
        f.pack();

        int windowWidth = f.getSize().width;
        int windowHeight = f.getSize().height;
        f.setLocation(Toolkit.getDefaultToolkit().getScreenSize().width / 2 - windowWidth / 2, Toolkit.getDefaultToolkit().getScreenSize().height / 2 - windowHeight / 2);
        f.setVisible(true);
    }

    private static String convert(String grid) {
        grid = grid.replace('1', '!');
        grid = grid.replace('2', '1');
        grid = grid.replace('!', '2');
        return grid;
    }

    private static void incItr() {
        System.out.println();
        int itr = Integer.parseInt(((JLabel)buttons.getComponent(3)).getText().substring(5));
        itr++;
        ((JLabel)buttons.getComponent(3)).setText("Itr: " + itr);
    }

    private static void updateTiles() {
        for (int i = 0; i < gridSize; i++) {
            for (int j = 0; j < gridSize; j++) {
                JPanel pp = (JPanel) tiles.getComponent((i * gridSize) + j);

                if (grid.grid[i][j] == 0) {
                    pp.setBackground(new Color(240, 240, 240));
                } else if (grid.grid[i][j] == 1) {
                    pp.setBackground(Color.red);
                } else if (grid.grid[i][j] == 2) {
                    pp.setBackground(Color.blue);
                }
                RepaintManager.currentManager(pp).markCompletelyClean(pp);
            }
        }
        RepaintManager.currentManager(tiles).markCompletelyDirty(tiles);
    }

    private static boolean thresholdReached() {
        int count = 0;
        for (int i = 0; i < gridSize; i += tileSize) {
            for (int j = 0; j < gridSize; j += tileSize) {
                for (int ii = i; ii < i + tileSize && ii < gridSize; ii++) {
                    for (int jj = j; jj < j + tileSize && jj < gridSize; jj++) {
                        if (grid.grid[ii][jj] != 0) {
                            count++;
                        }
                    }
                }
                if ((double) count / (double) (tileSize * tileSize) > threshold) {
                    return true;
                }
                count = 0;
            }
        }
        return false;
    }

    private static class beginAnimation implements Runnable {

        public void run() {
            looping = 1;

            while (looping == 1 && !thresholdReached()) {
                try {
                    Thread.sleep((int) (1000 / fps));
                } catch (Exception e) {
                }
                grid.next();
                updateTiles();
                incItr();
            }
        }
    }
}
