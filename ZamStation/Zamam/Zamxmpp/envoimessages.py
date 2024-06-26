#!/usr/bin/env python3

# Slixmpp: The Slick XMPP Library
# Copyright (C) 2010  Nathanael C. Fritz
# This file is part of Slixmpp.
# See the file LICENSE for copying permission.

import logging
import time
from getpass import getpass
from argparse import ArgumentParser
import sys
import slixmpp
import threading
import os


class SendMessages(slixmpp.ClientXMPP):

    """
    A basic Slixmpp bot that will log in
    python send_client.py -d -j oneshot@example.com -t someone@example.net -m "This is a message"
    """
    recipient=None

    def __init__(self, jid, password,recipient,message):
        self.recipient = recipient
        self.msg = message
        slixmpp.ClientXMPP.__init__(self, jid, password)

        # The message we wish to send, and the JID that
        # will receive it.
        # The session_start event will be triggered when
        # the bot establishes its connection with the server
        # and the XML streams are ready for use. We want to
        # listen for this event so that we we can initialize
        # our roster.
        self.add_event_handler("session_start", self.start)

    async def start(self, event):
        """
        Process the session_start event.

        Typical actions for the session_start event are
        requesting the roster and broadcasting an initial
        presence stanza.

        Arguments:
            event -- An empty dictionary. The session_start
                     event does not provide any additional
                     data.
        """
       
        self.send_presence()
        await self.get_roster()
        
        self.send_message(mto=self.recipient,
                          mbody=self.msg,
                          mtype='chat')

        self.disconnect()
        print("EXIT thread")
        await self.stop()
        

    async def stop(self):
        raise Exception("stop pid={0}".format(pid))


if __name__ == '__main__':
    # Setup the command line arguments.
    try:

        #ff=open("/home/pi/logs/Zamreportxmpptoterr.log",'a')
        #sys.stderr=ff
        #kk=open("totout.log",'w')
        #sys.strout=kk
        args = sys.argv  # liste des parametre d'appel
        pid=threading.get_ident()
        #print(" envoi message pid={0}".format(pid))
        parser = ArgumentParser(description=SendMessages.__doc__)

        # Output verbosity options.
        parser.add_argument("-q", "--quiet", help="set logging to ERROR",
                            action="store_const", dest="loglevel",
                            const=logging.ERROR, default=logging.INFO)
        parser.add_argument("-d", "--debug", help="set logging to DEBUG",
                            action="store_const", dest="loglevel",
                            const=logging.DEBUG, default=logging.INFO)

        # JID and password options.
        parser.add_argument("-j", "--jid", dest="jid",
                            help="JID to use")
        parser.add_argument("-p", "--password", dest="password",
                            help="password to use")
        parser.add_argument("-t", "--to", dest="to",
                            help="JID to send the message to")
        parser.add_argument("-m", "--message", dest="message",
                            help="message to send")

        args = parser.parse_args()

        # Setup logging.
        logging.basicConfig(level=args.loglevel,
                            format='%(levelname)-8s %(message)s')

        if args.jid is None:
            args.jid = input("Username: ")
        if args.password is None:
            args.password = getpass("Password: ")
        if args.to is None:
            args.to = input("Send To: ")
        if args.message is None:
            args.message = input("Message: ")

        # Setup the EchoBot and register plugins. Note that while plugins may
        # have interdependencies, the order in which you register them does
        # not matter.
        xmpp = SendMessages(args.jid, args.password,args.to,args.message)
        xmpp.register_plugin('xep_0030') # Service Discovery
        xmpp.register_plugin('xep_0199') # XMPP Ping
        # Connect to the XMPP server and start processing XMPP stanzas.
        xmpp.connect()
        xmpp.process()
        prov="prov"
    except Exception as ee:
        pass
