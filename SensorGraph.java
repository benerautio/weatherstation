import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Scanner;

import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JPanel;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;

import com.fazecast.jSerialComm.SerialPort;



public class SensorGraph {
	
	static SerialPort chosenPort;
	
	static int x=0;
	
	public static void main(String[] args) {
		JFrame window = new JFrame();
		window.setTitle("Temperature Sensor GUI");
		window.setSize(600,400);
		window.setLayout(new BorderLayout());
		window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE );
		
		JFrame window2 = new JFrame();
		window2.setTitle("Humidity Sensor GUI");
		window2.setSize(600,400);
		window2.setLayout(new BorderLayout());
		window2.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE );
		
		JFrame window3 = new JFrame();
		window3.setTitle("UV Sensor GUI");
		window3.setSize(600,400);
		window3.setLayout(new BorderLayout());
		window3.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE );
	
		JComboBox<String> portlist = new JComboBox<String>();
		JButton connectionButton = new JButton("Connect");
		JPanel topPanel = new JPanel();
		topPanel.add(portlist);
		topPanel.add(connectionButton);
		window.add(topPanel,BorderLayout.NORTH);
		
		SerialPort[] portNames = SerialPort.getCommPorts();
		for(int i = 0; i<portNames.length; i++) {
			portlist.addItem(portNames[i].getSystemPortName());
		}
		
		XYSeries series = new XYSeries("Temperature Readings");
		XYSeriesCollection dataset = new XYSeriesCollection(series);
		JFreeChart chart = ChartFactory.createXYLineChart("TMP36 Readings", "Time (s)", "Temperature (f)", dataset);
		window.add(new ChartPanel(chart), BorderLayout.CENTER);
		
		XYSeries series2 = new XYSeries("RHT03 Readings");
		XYSeriesCollection dataset2 = new XYSeriesCollection(series2);
		JFreeChart chart2 = ChartFactory.createXYLineChart("Humidity Readings", "Time (s)", "Humidity (%)", dataset2);
		window2.add(new ChartPanel(chart2), BorderLayout.CENTER);
		
		XYSeries series3 = new XYSeries("ML8511 Readings");
		XYSeriesCollection dataset3 = new XYSeriesCollection(series3);
		JFreeChart chart3 = ChartFactory.createXYLineChart("UV Intensity Readings", "Time (s)", "Intensity (mW/cm^2)", dataset3);
		window3.add(new ChartPanel(chart3), BorderLayout.CENTER);
		
		connectionButton.addActionListener(new ActionListener() {
			@Override public void actionPerformed(ActionEvent arg0) {
				if(connectionButton.getText().equals("Connect")) {
					chosenPort = SerialPort.getCommPort(portlist.getSelectedItem().toString());
					chosenPort.setComPortTimeouts(SerialPort.TIMEOUT_SCANNER, 0, 0);
					if(chosenPort.openPort()) {
						connectionButton.setText("Disconnect");
						portlist.setEnabled(false);
						
					}
					
					Thread thread = new Thread() {
						@Override public void run() {
							Scanner scanner = new Scanner(chosenPort.getInputStream());
							
							int j=0;
							while(scanner.hasNextLine()) {
								
								try {
									String line = scanner.nextLine();
									String[] tokens = line.split(",");
									double[] samples = new double[tokens.length];
									for(int i = 0; i < tokens.length; i++)
										samples[i] = Double.parseDouble(tokens[i]);
									if (j>0) {
									series.add(x=x+7,samples[0]);
									series2.add(x,samples[1]);
									series3.add(x,samples[2]);
									}
									j++;
									//System.out.println(samples[0]);
									//System.out.println(samples[1]);
									//System.out.println(samples[2]);
									System.out.println("Temperature (f): "+samples[0]+ " | Humidity (%): "+samples[1]+" | UV Intensity (mW/cm^2): "+samples[2]);
									//int number = Integer.parseInt(line);
									//series.add(x=x+7, number);	//edit this
									window.repaint();
									window2.repaint();
									
								}
								
								catch(Exception e) {}	
							}
							scanner.close();
						}
					};
					thread.start();
				}
				else {
					chosenPort.closePort();
					portlist.setEnabled(true);
					connectionButton.setText("Connect");
					
				}
			}
		});
		
		window.setVisible(true);
		window2.setVisible(true);
		window3.setVisible(true);
		
	}

}
