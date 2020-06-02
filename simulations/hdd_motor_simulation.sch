<Qucs Schematic 0.0.19>
<Properties>
  <View=0,-300,1504,1942,1,0,300>
  <Grid=10,10,1>
  <DataSet=hdd_motor_simulation.dat>
  <DataDisplay=hdd_motor_simulation.dpl>
  <OpenDisplay=1>
  <Script=hdd_motor_simulation.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <L L2 1 860 470 10 -26 0 1 "1 nH" 1 "" 0>
  <L L3 1 970 560 -26 -48 0 2 "1 nH" 1 "" 0>
  <L L1 1 750 560 -26 10 0 0 "1 nH" 1 "" 0>
  <GND * 1 860 560 0 0 0 0>
  <GND * 1 1020 560 0 0 0 1>
  <GND * 1 860 420 0 0 0 2>
  <GND * 1 700 560 0 0 0 3>
  <GND * 1 860 900 0 0 0 0>
  <Vac V1 1 750 900 -26 -56 0 2 "1 V" 1 "1 Hz" 0 "0" 0 "0" 0>
  <VProbe Pr1 1 550 990 28 -31 0 0>
  <VProbe Pr2 1 670 1000 28 -31 0 0>
  <VProbe Pr3 1 780 1030 28 -31 0 0>
  <.TR TR1 1 180 780 0 65 0 0 "lin" 1 "0" 1 "3s" 1 "1001" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <OpAmp OP1 1 1310 870 -26 42 0 0 "1e6" 1 "15 V" 0>
  <OpAmp OP2 1 1310 1020 -26 42 0 0 "1e6" 1 "15 V" 0>
  <VProbe Pr4 1 1410 850 28 -31 0 0>
  <VProbe Pr5 1 1410 1000 28 -31 0 0>
  <GND * 1 1420 870 0 0 0 0>
  <GND * 1 1420 1020 0 0 0 0>
  <Vac V2 1 970 900 -26 18 0 0 "1 V" 1 "1 Hz" 0 "phase1" 1 "0" 0>
  <Vac V3 1 860 790 18 -26 0 1 "1 V" 1 "1 Hz" 0 "phase2" 1 "0" 0>
  <Eqn Eqn1 1 220 1040 -34 16 0 0 "spin=1" 1 "phase1=spin*120" 1 "phase2=spin*240" 1 "yes" 0>
</Components>
<Wires>
  <780 900 790 900 "" 0 0 0 "">
  <860 820 860 900 "" 0 0 0 "">
  <1000 560 1020 560 "" 0 0 0 "">
  <860 560 940 560 "" 0 0 0 "">
  <700 560 720 560 "" 0 0 0 "">
  <780 560 860 560 "" 0 0 0 "">
  <860 500 860 560 "" 0 0 0 "">
  <860 420 860 440 "" 0 0 0 "">
  <680 950 680 1020 "" 0 0 0 "">
  <680 950 790 950 "" 0 0 0 "">
  <790 900 860 900 "" 0 0 0 "">
  <790 900 790 950 "" 0 0 0 "">
  <680 1020 680 1110 "" 0 0 0 "">
  <680 1110 790 1110 "" 0 0 0 "">
  <790 1050 790 1110 "" 0 0 0 "">
  <560 1110 680 1110 "" 0 0 0 "">
  <560 1010 560 1110 "" 0 0 0 "">
  <540 1010 540 1050 "" 0 0 0 "">
  <540 1050 610 1050 "" 0 0 0 "">
  <610 900 610 1050 "" 0 0 0 "">
  <610 900 700 900 "" 0 0 0 "">
  <660 1020 660 1180 "" 0 0 0 "">
  <660 1180 1040 1180 "" 0 0 0 "">
  <1040 900 1040 1040 "" 0 0 0 "">
  <770 1050 770 1090 "" 0 0 0 "">
  <770 1090 910 1090 "" 0 0 0 "">
  <910 710 910 890 "" 0 0 0 "">
  <860 710 910 710 "" 0 0 0 "">
  <860 710 860 760 "" 0 0 0 "">
  <1040 1040 1040 1180 "" 0 0 0 "">
  <1040 1040 1280 1040 "" 0 0 0 "">
  <1280 980 1280 1000 "" 0 0 0 "">
  <860 980 1280 980 "" 0 0 0 "">
  <860 940 860 980 "" 0 0 0 "">
  <700 940 860 940 "" 0 0 0 "">
  <700 900 720 900 "" 0 0 0 "">
  <700 900 700 940 "" 0 0 0 "">
  <910 890 910 1090 "" 0 0 0 "">
  <910 890 1280 890 "" 0 0 0 "">
  <1280 830 1280 850 "" 0 0 0 "">
  <1090 830 1280 830 "" 0 0 0 "">
  <1090 830 1090 900 "" 0 0 0 "">
  <1040 900 1090 900 "" 0 0 0 "">
  <1350 870 1400 870 "" 0 0 0 "">
  <1350 1020 1400 1020 "" 0 0 0 "">
  <860 900 940 900 "" 0 0 0 "">
  <1000 900 1040 900 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>