<?xml version="1.0" encoding="UTF-8"?>
<SCL xmlns="http://www.iec.ch/61850/2003/SCL" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.iec.ch/61850/2003/SCL.xsd" version="2007" revision="B">
	<process>
		<substation name="ChargingStation"    desc="mockup ZAMIREN">		
			<voltageLevel name="V1">
				<bay name="DERCSD">
					<ConductingEquipment name="ECP1" type="IFL">
						<eqFunction name="ECP1" desc="connexion point of the Charging Station to the local grid ">
							<LNode  iedName="IEDCSD" ldInst="LDCS"  lnClass="DECP" lnType="DECPZ" lnInst="1" />
							<Terminal name="T1" connectivityNode="ChargingStation/V1/DERCSD/CN1" cNodeName="CN1"/>
						</eqFunction>
						<function name="powerManagementChargingStation">
							<LNode iedName="IEDCSD" ldInst="LDCS" lnClass="DPMC" lnType="DPMCZ" lnInst="1" />
							<LNode iedName="IEDCSD" ldInst="LDCS" lnClass="DSTO" lnType="DSTOCS" lnInst="1" />					
						</function>
					</ConductingEquipment>	
					<ConductingEquipment name="SW1" type="DIS"> 
						<LNode iedName="IEDCSD" ldInst="LDCS" lnClass="XSWI" lnType="XSWIZ" lnInst="1" />
						<LNode iedName="IEDCSD" ldInst="LDCS" lnClass="MMXU" lnType="MMXUZ" lnInst="1"/>
						<Terminal name="T2" connectivityNode="ChargingStation/V1/DERCS/CN1" cNodeName="CN1"/>	
						<Terminal name="T3" connectivityNode="ChargingStation/V1/BSB1/CN2" cNodeName="CN2"/>	
					</ConductingEquipment>	
					<LNode iedName="IEDCSD" ldInst="LDCS" lnClass="FSCH" lnType="FSCHP" lnInst="1" />
					<LNode iedName="IEDCSD" ldInst="LDCS" lnClass="FSCH" lnType="FSCHP" lnInst="2" />
					<ConnectivityNode name="CN1" pathName="ChargingStation/V1/DERCSD/CN1"/>	
²					<bay name="CAB1">
						<ConductingEquipment name="CABLE1" type="CAB">
						</ConductingEquipment>
						<ConnectivityNode name="CN11" pathName="ChargingStation/V1/CAB1/CN6"/>
						<ConnectivityNode name="CN12" pathName="ChargingStation/V1/CAB1/CN8"/>
					</bay>
					<bay name="BSB1">
						<ConductingEquipment name="oubli" type="LIN">
							<ConnectivityNode name="CN2" pathName="ChargingStation/V1/busbar1/CN2"/>
						</ConductingEquipment>
					</bay>	
					<bay name="DEREVSE1">
						<ConductingEquipment name="ECP10" type="CAB">
							<eqFunction name="ECP10" desc="connexion point of the EVSE1 to the internal CS busbar ">
								<LNode iedName="IEDSE1" ldInst="LDEVSE1" lnClass="DECP" lnType="DECPZ" lnInst="2"/>
							</eqFunction>
							<Terminal name="T10" connectivityNode="ChargingStation/V1/DEREVSE1/CN4" cNodeName="CN10"/>	
							<Terminal name="T11" connectivityNode="ChargingStation/V1/BSB1/CN2" cNodeName="CN10"/>			
						</ConductingEquipment>
						<ConductingEquipment name="Socket1"	type="DIS"> 
							<LNode iedName="IEDSE1" ldInst="LDEVSE1" lnClass="XSWI" lnType="XSWIZ" lnInst="1" />
							<Terminal name="T12" connectivityNode="ChargingStation/V1/DEREVSE1/CN4" cNodeName="CN11"/>	
							<Terminal name="T13" connectivityNode="ChargingStation/V1/CAB1/CN6" cNodeName="CN12"/>		
						</ConductingEquipment>		
						<function name="powerManagementEVSE1">
							<LNode iedName="IEDSE1" ldInst="LDEVSE1" lnClass="DPMC" lnType="DPMCZ" lnInst="2" />
							<LNode iedName="IEDSE1" ldInst="LDEVSE1" lnClass="DSTO" lnType="DSTOEVSE" lnInst="2" />			
						</function>	
						<LNode iedName="IEDSE1" ldInst="LDEVSE1" lnClass="FSCH" lnType="FSCHZ" lnInst="1" />
						<LNode iedName="IEDSE1" ldInst="LDEVSE1" lnClass="FSCH" lnType="FSCHZ" lnInst="2" />
						<LNode iedName="IEDSE1" ldInst="LDEVSE1" lnClass="FSCC" lnType="FSCCZ" lnInst="1" />
						<LNode iedName="IEDSE1" ldInst="LDEVSE1" lnClass="DEAO" lnType="DEAOZ" lnInst="1" />
						<LNode iedName="IEDSE1" ldInst="LDEVSE1" lnClass="DEDO" lnType="DEDOZ" lnInst="1" />
						<LNode iedName="IEDSE1" ldInst="LDEVSE1" lnClass="DESE" lnType="DESEZ" lnInst="1" />
						<ConnectivityNode name="CN12" pathName="ChargingStation/V1/DEREVSE1/CN12"/>
						<bay name="DEREV1">
							<ConductingEquipment name="what?" type="CAB">
								<eqFunction name="ECP11" desc="connexion point of DEREV1">
									<LNode iedName="IEDEV1" ldInst="LDEV1" lnClass="DECP" lnType="DECPZ" lnInst="1" />
								</eqFunction>
								<Terminal name="T14" connectivityNode="ChargingStation/V1/CAB1/CN12" cNodeName="CN8"/>	
								<Terminal name="T15" connectivityNode="ChargingStation/V1/DEREV1/CN13" cNodeName="CN13"/>	
							</ConductingEquipment>
							<ConductingEquipment name="SW10" type="DIS"> 
								<LNode iedName="IEDEV1" ldInst="LDEV1" lnClass="XSWI" lnType="XSWIZ" lnInst="1" />
								<Terminal name="T16" connectivityNode="ChargingStation/V1/DEREV1/CN13" cNodeName="CN10"/>	
								<Terminal name="T18" connectivityNode="ChargingStation/V1/DEREV1/CN14" cNodeName="CN12"/>	
							</ConductingEquipment>		
							<ConductingEquipment name="IFL2" type="IFL" name2="RessourceDEREV1" desc="EV1">
								<EqFunction name="powerManagementDEREV1" >
									<LNode iedName="IEDEV1" ldInst="LDEV1" lnClass="DSTO" lnType="DSTOEV" lnInst="1" />
									<LNode iedName="IEDEV1" ldInst="LDEV1" lnClass="DPMC" lnType="DPMCZ" lnInst="1" />
								</EqFunction>
							<Terminal name="T18" connectivityNode="ChargingStation/V1/DEREV1/CN12" cNodeName="CN14"/>	
							</ConductingEquipment>	
							<LNode iedName="IEDEV1" ldInst="LDEV1" lnClass="FSCH" lnType="FSCHZ" lnInst="1" />
							<LNode iedName="IEDEV1" ldInst="LDEV1" lnClass="FSCH" lnType="FSCHZ" lnInst="2" />
							<LNode iedName="IEDEV1" ldInst="LDEV1" lnClass="FSCC" lnType="FSCCZ" lnInst="1" />
							<LNode iedName="IEDEV1" ldInst="LDEV1" lnClass="DEEV" lnType="DEEVZ" lnInst="1" />				
							<ConnectivityNode name="CN13" pathName="ChargingStation/V1/DEREV1/CN13"/>
							<ConnectivityNode name="CN14" pathName="ChargingStation/V1/DEREV1/CN14"/>
						</bay>
					</bay>
					<bay name="DEREVSE2">
						<ConductingEquipment name="ECP20" type="CAB">
							<eqFunction name="ECP20" desc="connexion point of the EVSE1 to the internal CS busbar ">
								<LNode iedName="IEDSE2" ldInst="LDEVSE2" lnClass="DECP" lnType="DECPZ" lnInst="2"/>
							</eqFunction>
							<Terminal name="T20" connectivityNode="ChargingStation/V1/DEREVSE1/CN20" cNodeName="CN10"/>	
							<Terminal name="T21" connectivityNode="ChargingStation/V1/BSB1/CN21" cNodeName="CN21"/>			
						</ConductingEquipment>
						<ConductingEquipment name="Socket2"	type="DIS"> 
							<LNode iedName="IEDSE2" ldInst="LDEVSE2" lnClass="XSWI" lnType="XSWIZ" lnInst="2" />
							<Terminal name="T22" connectivityNode="ChargingStation/V1/DEREVSE1/CN22" cNodeName="CN22"/>	
							<Terminal name="T23" connectivityNode="ChargingStation/V1/CAB1/CN23" cNodeName="CN23"/>		
						</ConductingEquipment>		
						<function name="powerManagementEVSE2">
							<LNode iedName="IEDSE2" ldInst="LDEVSE2" lnClass="DPMC" lnType="DPMCZ" lnInst="2" />
							<LNode iedName="IEDSE2" ldInst="LDEVSE2" lnClass="DSTO" lnType="DSTOEVSE" lnInst="2" />			
						</function>	
						<LNode iedName="IEDSE2" ldInst="LDEVSE2" lnClass="FSCH" lnType="FSCHZ" lnInst="1" />
						<LNode iedName="IEDSE2" ldInst="LDEVSE2" lnClass="FSCH" lnType="FSCHZ" lnInst="2" />
						<LNode iedName="IEDSE2" ldInst="LDEVSE2" lnClass="FSCC" lnType="FSCCZ" lnInst="1" />
						<LNode iedName="IEDSE2" ldInst="LDEVSE2" lnClass="DEAO" lnType="DEAOZ" lnInst="1" />
						<LNode iedName="IEDSE2" ldInst="LDEVSE2" lnClass="DEDO" lnType="DEDOZ" lnInst="1" />
						<LNode iedName="IEDSE2" ldInst="LDEVSE2" lnClass="DESE" lnType="DESEZ" lnInst="1" />
						<ConnectivityNode name="CN22" pathName="ChargingStation/V1/DEREVSE1/CN22"/>
						<bay name="DEREV2">
							<ConductingEquipment name="cable2" type="CAB">
								<eqFunction name="ECP21" desc="connexion point of DEREV2">
									<LNode iedName="IEDEV1" ldInst="LDEV1" lnClass="DECP" lnType="DECPZ" lnInst="1" />
								</eqFunction>
								<Terminal name="T24" connectivityNode="ChargingStation/V1/CAB1/CN22" cNodeName="CN22"/>	
								<Terminal name="T25" connectivityNode="ChargingStation/V1/DEREV1/CN23" cNodeName="CN23"/>	
							</ConductingEquipment>
							<ConductingEquipment name="SW10" type="DIS"> 
								<LNode iedName="IEDEV2" ldInst="LDEV1" lnClass="XSWI" lnType="XSWIZ" lnInst="1" />
								<Terminal name="T26" connectivityNode="ChargingStation/V1/DEREV1/CN13" cNodeName="CN20"/>	
								<Terminal name="T28" connectivityNode="ChargingStation/V1/DEREV1/CN14" cNodeName="CN22"/>	
							</ConductingEquipment>		
							<ConductingEquipment name="IFL2" type="IFL" name2="RessourceDEREV2" desc="EV2">
								<EqFunction name="powerManagementDEREV2" >
									<LNode iedName="IEDEV2" ldInst="LDEV1" lnClass="DSTO" lnType="DSTOEV" lnInst="1" />
									<LNode iedName="IEDEV2" ldInst="LDEV1" lnClass="DPMC" lnType="DPMCZ" lnInst="1" />
								</EqFunction>
							<Terminal name="T28" connectivityNode="ChargingStation/V1/DEREV1/CN28" cNodeName="CN24"/>	
							</ConductingEquipment>	
							<LNode iedName="IEDEV2" ldInst="LDEV1" lnClass="FSCH" lnType="FSCHZ" lnInst="1" />
							<LNode iedName="IEDEV2" ldInst="LDEV1" lnClass="FSCH" lnType="FSCHZ" lnInst="2" />
							<LNode iedName="IEDEV2" ldInst="LDEV1" lnClass="FSCC" lnType="FSCCZ" lnInst="1" />
							<LNode iedName="IEDEV2" ldInst="LDEV1" lnClass="DEEV" lnType="DEEVZ" lnInst="1" />				
							<ConnectivityNode name="CN23" pathName="ChargingStation/V1/DEREV1/CN23"/>
							<ConnectivityNode name="CN24" pathName="ChargingStation/V1/DEREV1/CN24"/>
						</bay>
					</bay>
					<bay name="DEREVSE3">
						<ConductingEquipment name="ECP30" type="CAB">
							<eqFunction name="ECP30" desc="connexion point of the EVSE3 to the internal CS busbar ">
								<LNode iedName="IEDSE3" ldInst="LDEVSE3" lnClass="DECP" lnType="DECPZ" lnInst="3"/>
							</eqFunction>
							<Terminal name="T30" connectivityNode="ChargingStation/V1/DEREVSE1/CN4" cNodeName="CN30"/>	
							<Terminal name="T31" connectivityNode="ChargingStation/V1/BSB1/CN2" cNodeName="CN30"/>			
						</ConductingEquipment>
						<ConductingEquipment name="Socket3"	type="DIS"> 
							<LNode iedName="IEDSE3" ldInst="LDEVSE3" lnClass="XSWI" lnType="XSWIZ" lnInst="1" />
							<Terminal name="T32" connectivityNode="ChargingStation/V1/DEREVSE3/CN31" cNodeName="CN31"/>	
							<Terminal name="T33" connectivityNode="ChargingStation/V1/CAB1/CN32" cNodeName="CN32"/>		
						</ConductingEquipment>		
						<function name="powerManagementEVSE3">
							<LNode iedName="IEDSE3" ldInst="LDEVSE1" lnClass="DPMC" lnType="DPMCZ" lnInst="2" />
							<LNode iedName="IEDSE3" ldInst="LDEVSE1" lnClass="DSTO" lnType="DSTOEVSE" lnInst="2" />			
						</function>	
						<LNode iedName="IEDSE3" ldInst="LDEVSE1" lnClass="FSCH" lnType="FSCHZ" lnInst="1" />
						<LNode iedName="IEDSE3" ldInst="LDEVSE1" lnClass="FSCH" lnType="FSCHZ" lnInst="2" />
						<LNode iedName="IEDSE3" ldInst="LDEVSE1" lnClass="FSCC" lnType="FSCCZ" lnInst="1" />
						<LNode iedName="IEDSE3" ldInst="LDEVSE1" lnClass="DEAO" lnType="DEAOZ" lnInst="1" />
						<LNode iedName="IEDSE3" ldInst="LDEVSE1" lnClass="DEDO" lnType="DEDOZ" lnInst="1" />
						<LNode iedName="IEDSE3" ldInst="LDEVSE1" lnClass="DESE" lnType="DESEZ" lnInst="1" />
						<ConnectivityNode name="CN32" pathName="ChargingStation/V1/DEREVSE1/CN32"/>
						<bay name="DEREV3">
							<ConductingEquipment name="cable3" type="CAB">
								<eqFunction name="ECP31" desc="connexion point of DEREV1">
									<LNode iedName="IEDEV3" ldInst="LDEV1" lnClass="DECP" lnType="DECPZ" lnInst="1" />
								</eqFunction>
								<Terminal name="T34" connectivityNode="ChargingStation/V1/CAB1/CN32" cNodeName="CN32"/>	
								<Terminal name="T35" connectivityNode="ChargingStation/V1/DEREV1/CN33" cNodeName="CN33"/>	
							</ConductingEquipment>
							<ConductingEquipment name="SW10" type="DIS"> 
								<LNode iedName="IEDEV3" ldInst="LDEV1" lnClass="XSWI" lnType="XSWIZ" lnInst="1" />
								<Terminal name="T36" connectivityNode="ChargingStation/V1/DEREV1/CN30" cNodeName="CN30"/>	
								<Terminal name="T38" connectivityNode="ChargingStation/V1/DEREV1/CN32" cNodeName="CN32"/>	
							</ConductingEquipment>		
							<ConductingEquipment name="IFL3" type="IFL" name2="RessourceDEREV3" desc="EV3">
								<EqFunction name="powerManagementDEREV3" >
									<LNode iedName="IEDEV3" ldInst="LDEV1" lnClass="DSTO" lnType="DSTOEV" lnInst="1" />
									<LNode iedName="IEDEV3" ldInst="LDEV1" lnClass="DPMC" lnType="DPMCZ" lnInst="1" />
								</EqFunction>
								<Terminal name="T38" connectivityNode="ChargingStation/V1/DEREV1/CN32" cNodeName="CN34"/>	
							</ConductingEquipment>	
							<LNode iedName="IEDEV3" ldInst="LDEV1" lnClass="FSCH" lnType="FSCHZ" lnInst="1" />
							<LNode iedName="IEDEV3" ldInst="LDEV1" lnClass="FSCH" lnType="FSCHZ" lnInst="2" />
							<LNode iedName="IEDEV3" ldInst="LDEV1" lnClass="FSCC" lnType="FSCCZ" lnInst="1" />
							<LNode iedName="IEDEV3" ldInst="LDEV1" lnClass="DEEV" lnType="DEEVZ" lnInst="1" />				
							<ConnectivityNode name="CN33" pathName="ChargingStation/V1/DEREV1/CN33"/>
							<ConnectivityNode name="CN34" pathName="ChargingStation/V1/DEREV1/CN34"/>
						</bay>
					</bay>
				</bay>
			</voltageLevel>
		</substation>
	</process>
	<Communication>
		<SubNetwork name="SubNet1" type="8-MMS">
			<Text/>
			<ConnectedAP iedName="IEDCSD" apName="Station1">
				<Address>
					<P type="IP">10.3.141.120</P>
					<P type="IP-SUBNET">255.255.255.0</P>
					<P type="IP-GATEWAY">10.3.141.1</P>
					<P type="OSI-TSEL">0001</P>
					<P type="OSI-SSEL">0001</P>
					<P type="OSI-PSEL">00000001</P>
					<P type="VLAN-ID">001</P>
				</Address>
			</ConnectedAP>	 
		</SubNetwork>
		<SubNetwork name="SubNet2" type="8-MMS">
		  <Text/>
		  <ConnectedAP iedName="IEDSE1" apName="EVSE1">
			<Address>
			  <P type="IP">10.3.141.121</P>
			  <P type="IP-SUBNET">255.255.255.0</P>
			  <P type="IP-GATEWAY">10.3.141.1</P>
			  <P type="OSI-TSEL">0001</P>
			  <P type="OSI-SSEL">0001</P>
			  <P type="OSI-PSEL">00000001</P>
			  <P type="VLAN-ID">001</P>
			</Address>
		  </ConnectedAP>
		</SubNetwork>
		<SubNetwork name="SubNet3" type="8-MMS">
		  <Text/>
		  <ConnectedAP iedName="IEDSE2" apName="EVSE2">
			<Address>
			  <P type="IP">10.3.141.122</P>
			  <P type="IP-SUBNET">255.255.255.0</P>
			  <P type="IP-GATEWAY">10.3.141.1</P>
			  <P type="OSI-TSEL">0001</P>
			  <P type="OSI-SSEL">0001</P>
			  <P type="OSI-PSEL">00000001</P>
			  <P type="VLAN-ID">001</P>
			</Address>
		  </ConnectedAP>
		</SubNetwork>
		<SubNetwork name="SubNet4" type="8-MMS">
		  <Text/>
		  <ConnectedAP iedName="IEDSE3" apName="EVSE3">
			<Address>
			  <P type="IP">10.3.141.123</P>
			  <P type="IP-SUBNET">255.255.255.0</P>
			  <P type="IP-GATEWAY">10.3.141.1</P>
			  <P type="OSI-TSEL">0001</P>
			  <P type="OSI-SSEL">0001</P>
			  <P type="OSI-PSEL">00000001</P>
			  <P type="VLAN-ID">001</P>
			</Address>
		  </ConnectedAP>
		</SubNetwork>
		<SubNetwork name="SubNetA" type="8-MMS">
			<Text/>
			<ConnectedAP iedName="IEDEV1" apName="AccPoint-V1">
				<Address>
					<P type="IP">0.0.0.0</P>
					<P type="IP-SUBNET">255.255.255.0</P>
					<P type="IP-GATEWAY">10.3.141.1</P>
					<P type="OSI-TSEL">0001</P>
					<P type="OSI-SSEL">0001</P>
					<P type="OSI-PSEL">00000001</P>
					<P type="VLAN-ID">001</P>
				</Address>
			</ConnectedAP>
		</SubNetwork>
		<SubNetwork name="SubNetB" type="8-MMS">
			<Text/>
			<ConnectedAP iedName="IEDEV2" apName="AccPoint-V2">
				<Address>
				   <P type="IP">0.0.0.0</P>
				   <P type="IP-SUBNET">255.255.255.0</P>
				   <P type="IP-GATEWAY">10.3.141.1</P>
				   <P type="OSI-TSEL">0001</P>
				   <P type="OSI-SSEL">0001</P>
				   <P type="OSI-PSEL">00000001</P>
				   <P type="VLAN-ID">001</P>
				</Address>
			</ConnectedAP>
		</SubNetwork>
		<SubNetwork name="SubNetC" type="8-MMS">
			<Text/>
			<ConnectedAP iedName="IEDEV3" apName="AccPoint-V1">
				<Address>
					<P type="IP">0.0.0.0</P>
					<P type="IP-SUBNET">255.255.255.0</P>
					<P type="IP-GATEWAY">10.3.141.1</P>
					<P type="OSI-TSEL">0001</P>
					<P type="OSI-SSEL">0001</P>
					<P type="OSI-PSEL">00000001</P>
					<P type="VLAN-ID">001</P>
				</Address>
			</ConnectedAP>
		</SubNetwork>
	</Communication>
</SCL>