/*
 * h323.cxx
 *
 * H.323 protocol handler
 *
 * Open H323 Library
 *
 * Copyright (c) 1998-2000 Equivalence Pty. Ltd.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Open H323 Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: h323.cxx,v $
 * Revision 1.2003  2001/08/13 05:10:39  robertj
 * Updates from OpenH323 v1.6.0 release.
 *
 * Revision 2.1  2001/08/01 05:15:22  robertj
 * Moved default session ID's to OpalMediaFormat class.
 * Added setting of connections capabilities from other connections in
 *   calls media format list.
 *
 * Revision 2.0  2001/07/27 15:48:25  robertj
 * Conversion of OpenH323 to Open Phone Abstraction Library (OPAL)
 *
 * Revision 1.190  2001/08/13 01:27:03  robertj
 * Changed GK admission so can return multiple aliases to be used in
 *   setup packet, thanks Nick Hoath.
 *
 * Revision 1.189  2001/08/06 03:08:56  robertj
 * Fission of h323.h to h323ep.h & h323con.h, h323.h now just includes files.
 *
 * Revision 1.188  2001/08/03 03:49:57  craigs
 * Fixed problem with AnswerCallDeferredWithMedia not working with fastStart
 * on some machine
 *
 * Revision 1.187  2001/08/02 04:32:17  robertj
 * Added ability for AdmissionRequest to alter destination alias used in
 *   the outgoing call. Thanks Ben Madsen & Graeme Reid.
 *
 * Revision 1.186  2001/08/01 00:46:16  craigs
 * Added ability to early start without Alerting
 *
 * Revision 1.185  2001/07/19 09:29:47  robertj
 * Adde X.880 invoke reject if get global opcode.
 *
 * Revision 1.184  2001/07/17 04:44:31  robertj
 * Partial implementation of T.120 and T.38 logical channels.
 *
 * Revision 1.183  2001/07/17 04:13:06  robertj
 * Fixed subtle bug when have split codec conflict, did not use the remotes
 *   caabilities (used local) so end up with incorrect frames per packet.
 *
 * Revision 1.182  2001/07/06 02:28:25  robertj
 * Moved initialisation of local capabilities back to constructor for
 *   backward compatibility reasons.
 *
 * Revision 1.181  2001/07/05 04:19:13  robertj
 * Added call back for setting local capabilities.
 *
 * Revision 1.180  2001/07/04 09:02:07  robertj
 * Added more tracing
 *
 * Revision 1.179  2001/06/14 07:10:35  robertj
 * Removed code made redundent by previous change.
 *
 * Revision 1.178  2001/06/14 06:25:16  robertj
 * Added further H.225 PDU build functions.
 * Moved some functionality from connection to PDU class.
 *
 * Revision 1.177  2001/06/13 06:38:25  robertj
 * Added early start (media before connect) functionality.
 *
 * Revision 1.176  2001/06/05 03:14:41  robertj
 * Upgraded H.225 ASN to v4 and H.245 ASN to v7.
 *
 * Revision 1.175  2001/05/31 07:16:56  robertj
 * Improved debug output on decoding internal PDU's, especially if fails.
 * Added trace dump of H4501 PDU's
 *
 * Revision 1.174  2001/05/30 23:34:54  robertj
 * Added functions to send TCS=0 for transmitter side pause.
 *
 * Revision 1.173  2001/05/22 00:40:20  robertj
 * Fixed restart problem when in transmitter paused (TCS=0) state, thanks Paul van de Wijngaard
 *
 * Revision 1.172  2001/05/17 07:11:29  robertj
 * Added more call end types for common transport failure modes.
 *
 * Revision 1.171  2001/05/17 03:30:45  robertj
 * Fixed support for transmiter side paused (TCS=0), thanks Paul van de Wijngaard
 *
 * Revision 1.170  2001/05/10 02:09:55  robertj
 * Added more call end codes from Q.931, no answer and no response.
 * Changed call transfer call end code to be the same as call forward.
 *
 * Revision 1.169  2001/05/09 04:59:04  robertj
 * Bug fixes in H.450.2, thanks Klein Stefan.
 *
 * Revision 1.168  2001/05/09 04:07:55  robertj
 * Added more call end codes for busy and congested.
 *
 * Revision 1.167  2001/05/03 06:49:43  robertj
 * Added bullet proofing for if get decode error in Q.931 User-User data.
 *
 * Revision 1.166  2001/05/02 16:22:21  rogerh
 * Add IsAllow() for a single capability to check if it is in the
 * capabilities set. This fixes the bug where OpenH323 would accept
 * incoming H261 video even when told not to accept it.
 *
 * Revision 1.165  2001/05/01 04:34:11  robertj
 * Changed call transfer API slightly to be consistent with new hold function.
 *
 * Revision 1.164  2001/05/01 02:12:50  robertj
 * Added H.450.4 call hold (Near End only), thanks David M. Cassel.
 *
 * Revision 1.163  2001/04/23 01:31:15  robertj
 * Improved the locking of connections especially at shutdown.
 *
 * Revision 1.162  2001/04/20 02:32:07  robertj
 * Improved logging of bandwith, used more intuitive units.
 *
 * Revision 1.161  2001/04/20 02:16:53  robertj
 * Removed GNU C++ warnings.
 *
 * Revision 1.160  2001/04/11 03:01:29  robertj
 * Added H.450.2 (call transfer), thanks a LOT to Graeme Reid & Norwood Systems
 *
 * Revision 1.159  2001/04/09 08:47:28  robertj
 * Added code to force the bringing up of a separate H.245 channel if not
 *   tunnelling and the call receiver does not start separate channel.
 *
 * Revision 1.158  2001/04/04 03:13:12  robertj
 * Made sure tunnelled H.245 is not used if separate channel is opened.
 * Made sure H.245 TCS & MSD is done if initiated the call and fast started,
 *   do so on receipt of the connect from remote end.
 * Utilised common function for TCS and MSD initiation.
 *
 * Revision 1.157  2001/03/08 07:45:06  robertj
 * Fixed issues with getting media channels started in some early start
 *   regimes, in particular better Cisco compatibility.
 *
 * Revision 1.156  2001/03/06 04:44:47  robertj
 * Fixed problem where could send capability set twice. This should not be
 *   a problem except when talking to another broken stack, eg Cisco routers.
 *
 * Revision 1.155  2001/02/09 05:13:55  craigs
 * Added pragma implementation to (hopefully) reduce the executable image size
 * under Linux
 *
 * Revision 1.154  2001/01/19 06:56:58  robertj
 * Fixed possible nested mutex of H323Connection if get PDU which waiting on answer.
 *
 * Revision 1.153  2001/01/11 06:40:37  craigs
 * Fixed problem with ClearCallSychronous failing because ClearCall was
 * called before it finished
 *
 * Revision 1.152  2001/01/03 05:59:07  robertj
 * Fixed possible deadlock that caused asserts about transport thread
 *    not terminating when clearing calls.
 *
 * Revision 1.151  2000/12/19 22:33:44  dereks
 * Adjust so that the video channel is used for reading/writing raw video
 * data, which better modularizes the video codec.
 *
 * Revision 1.150  2000/12/18 08:59:20  craigs
 * Added ability to set ports
 *
 * Revision 1.149  2000/11/27 02:44:06  craigs
 * Added ClearCall Synchronous to H323Connection and H323Endpoint to
 * avoid race conditions with destroying descendant classes
 *
 * Revision 1.148  2000/11/24 10:53:59  robertj
 * Fixed bug in fast started G.711 codec not working in one direction.
 *
 * Revision 1.147  2000/11/15 00:14:06  robertj
 * Added missing state to FastStartState strings, thanks Ulrich Findeisen
 *
 * Revision 1.146  2000/11/12 23:49:16  craigs
 * Added per connection versions of OnEstablished and OnCleared
 *
 * Revision 1.145  2000/11/08 04:44:02  robertj
 * Added function to be able to alter/remove the call proceeding PDU.
 *
 * Revision 1.144  2000/10/30 03:37:54  robertj
 * Fixed opeing of separate H.245 channel if fast start but not tunneling.
 *
 * Revision 1.143  2000/10/19 04:09:03  robertj
 * Fix for if early start opens channel which fast start still pending.
 *
 * Revision 1.142  2000/10/16 09:51:38  robertj
 * Fixed problem with not opening fast start video receive if do not have transmit enabled.
 *
 * Revision 1.141  2000/10/13 02:16:04  robertj
 * Added support for Progress Indicator Q.931/H.225 message.
 *
 * Revision 1.140  2000/10/12 05:11:54  robertj
 * Added trace log if get transport error on writing PDU.
 *
 * Revision 1.139  2000/10/04 12:21:06  robertj
 * Changed setting of callToken in H323Connection to be as early as possible.
 *
 * Revision 1.138  2000/10/04 05:58:42  robertj
 * Minor reorganisation of the H.245 secondary channel start up to make it simpler
 *    to override its behaviour.
 * Moved OnIncomingCall() so occurs before fast start elements are processed
 *    allowing them to be manipulated before the default processing.
 *
 * Revision 1.137  2000/09/25 06:48:11  robertj
 * Removed use of alias if there is no alias present, ie only have transport address.
 *
 * Revision 1.136  2000/09/22 04:06:12  robertj
 * Fixed race condition when attempting fast start and remote refuses both.
 *
 * Revision 1.135  2000/09/22 01:35:49  robertj
 * Added support for handling LID's that only do symmetric codecs.
 *
 * Revision 1.134  2000/09/22 00:32:33  craigs
 * Added extra logging
 * Fixed problems with no fastConnect with tunelling
 *
 * Revision 1.133  2000/09/20 01:50:21  craigs
 * Added ability to set jitter buffer on a per-connection basis
 *
 * Revision 1.132  2000/09/05 01:16:19  robertj
 * Added "security" call end reason code.
 *
 * Revision 1.131  2000/08/23 05:49:58  robertj
 * Fixed possible deadlock on clean up of connection and error on media channel occurs.
 *
 * Revision 1.130  2000/08/21 12:37:14  robertj
 * Fixed race condition if close call just as slow start media channels are opening, part 2.
 *
 * Revision 1.129  2000/08/21 02:50:28  robertj
 * Fixed race condition if close call just as slow start media channels are opening.
 *
 * Revision 1.128  2000/08/10 02:48:52  craigs
 * Fixed problem with operator precedence causing funny problems with codec selection
 *
 * Revision 1.127  2000/08/01 11:27:36  robertj
 * Added more tracing for user input.
 *
 * Revision 1.126  2000/07/31 14:08:18  robertj
 * Added fast start and H.245 tunneling flags to the H323Connection constructor so can
 *    disabled these features in easier manner to overriding virtuals.
 *
 * Revision 1.125  2000/07/15 09:56:37  robertj
 * Changed adding of fake remote capability to after fast start element has been
 *   checked and new parameters (eg frames per packet) has been parsed out of it.
 *
 * Revision 1.124  2000/07/14 14:03:47  robertj
 * Changed fast start connect so if has bad h245 address does not clear call.
 *
 * Revision 1.123  2000/07/13 16:07:22  robertj
 * Fixed ability to receive a H.245 secondary TCP link in a Connect during a fast start.
 *
 * Revision 1.122  2000/07/13 12:35:13  robertj
 * Fixed problems with fast start frames per packet adjustment.
 * Split autoStartVideo so can select receive and transmit independently
 *
 * Revision 1.121  2000/07/10 16:06:59  robertj
 * Added TCS=0 support.
 * Fixed bug where negotiations hang if not fast start and tunnelled but remot does not tunnel.
 *
 * Revision 1.120  2000/07/09 15:24:42  robertj
 * Removed sending of MSD in Setup PDU, is not to spec.
 * Added start of TCS if received TCS from other end and have not already started.
 *
 * Revision 1.119  2000/07/08 19:49:27  craigs
 * Fixed stupidity in handling of fastStart capabilities
 *
 * Revision 1.118  2000/07/04 09:00:54  robertj
 * Added traces for indicating failure of channel establishment due to capability set rules.
 *
 * Revision 1.117  2000/07/04 04:15:38  robertj
 * Fixed capability check of "combinations" for fast start cases.
 *
 * Revision 1.116  2000/07/04 01:16:49  robertj
 * Added check for capability allowed in "combinations" set, still needs more done yet.
 *
 * Revision 1.115  2000/06/17 09:14:13  robertj
 * Moved sending of DRQ to after release complete to be closer to spec, thanks Thien Nguyen
 *
 * Revision 1.114  2000/06/07 05:48:06  robertj
 * Added call forwarding.
 *
 * Revision 1.113  2000/06/05 06:33:08  robertj
 * Fixed problem with roud trip time statistic not being calculated if constant traffic.
 *
 * Revision 1.112  2000/06/03 03:16:39  robertj
 * Fixed using the wrong capability table (should be connections) for some operations.
 *
 * Revision 1.111  2000/05/23 11:32:36  robertj
 * Rewrite of capability table to combine 2 structures into one and move functionality into that class
 *    allowing some normalisation of usage across several applications.
 * Changed H323Connection so gets a copy of capabilities instead of using endponts, allows adjustments
 *    to be done depending on the remote client application.
 *
 * Revision 1.110  2000/05/22 05:21:36  robertj
 * Fixed race condition where controlChannel variable could be used before set.
 *
 * Revision 1.109  2000/05/16 08:14:30  robertj
 * Added function to find channel by session ID, supporting H323Connection::FindChannel() with mutex.
 * Added function to get a logical channel by channel number.
 *
 * Revision 1.108  2000/05/16 02:06:00  craigs
 * Added access functions for particular sessions
 *
 * Revision 1.107  2000/05/09 12:19:31  robertj
 * Added ability to get and set "distinctive ring" Q.931 functionality.
 *
 * Revision 1.106  2000/05/09 04:12:36  robertj
 * Changed fast start to offer multiple codecs instead of only the preferred one.
 *
 * Revision 1.105  2000/05/08 14:32:26  robertj
 * Fixed GNU compiler compatibility problems.
 *
 * Revision 1.104  2000/05/08 14:07:35  robertj
 * Improved the provision and detection of calling and caller numbers, aliases and hostnames.
 *
 * Revision 1.103  2000/05/05 05:05:55  robertj
 * Removed warning in GNU compiles.
 *
 * Revision 1.102  2000/05/02 04:32:26  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.101  2000/04/28 13:01:44  robertj
 * Fixed problem with adjusting tx/rx frame counts in capabilities during fast start.
 *
 * Revision 1.100  2000/04/18 23:10:49  robertj
 * Fixed bug where fast starts two video recievers instread of receiver and transmitter.
 *
 * Revision 1.99  2000/04/14 21:09:51  robertj
 * Work around for compatibility problem wth broken Altigen AltaServ-IG PBX.
 *
 * Revision 1.98  2000/04/14 20:03:05  robertj
 * Added function to get remote endpoints application name.
 *
 * Revision 1.97  2000/04/14 17:31:05  robertj
 * Fixed bug where checking the error code on the wrong channel. Caused hang ups.
 *
 * Revision 1.96  2000/04/13 18:08:46  robertj
 * Fixed error in SendUserInput() check for value string, previous change was seriously broken.
 *
 * Revision 1.95  2000/04/11 20:07:01  robertj
 * Added missing trace strings for new call end reasons on gatekeeper denied calls.
 *
 * Revision 1.94  2000/04/11 04:02:48  robertj
 * Improved call initiation with gatekeeper, no longer require @address as
 *    will default to gk alias if no @ and registered with gk.
 * Added new call end reasons for gatekeeper denied calls.
 *
 * Revision 1.93  2000/04/10 20:37:33  robertj
 * Added support for more sophisticated DTMF and hook flash user indication.
 *
 * Revision 1.92  2000/04/06 17:50:16  robertj
 * Added auto-start (including fast start) of video channels, selectable via boolean on the endpoint.
 *
 * Revision 1.91  2000/04/05 03:17:31  robertj
 * Added more RTP statistics gathering and H.245 round trip delay calculation.
 *
 * Revision 1.90  2000/03/29 04:42:19  robertj
 * Improved some trace logging messages.
 *
 * Revision 1.89  2000/03/29 02:14:45  robertj
 * Changed TerminationReason to CallEndReason to use correct telephony nomenclature.
 * Added CallEndReason for capability exchange failure.
 *
 * Revision 1.88  2000/03/27 22:43:11  robertj
 * Fixed possible write to closed channel on shutdown when control channel separate.
 *
 * Revision 1.87  2000/03/25 02:01:50  robertj
 * Added adjustable caller name on connection by connection basis.
 *
 * Revision 1.86  2000/03/22 01:30:34  robertj
 * Fixed race condition in accelerated start (Cisco compatibility).
 *
 * Revision 1.85  2000/03/21 01:43:59  robertj
 * Fixed (faint) race condition when starting separate H.245 channel.
 *
 * Revision 1.84  2000/03/21 01:08:10  robertj
 * Fixed incorrect call reference code being used in originated call.
 *
 * Revision 1.83  2000/03/07 13:54:28  robertj
 * Fixed assert when cancelling call during TCP connect, thanks Yura Ershov.
 *
 * Revision 1.82  2000/03/02 02:18:13  robertj
 * Further fixes for early H245 establishment confusing the fast start code.
 *
 * Revision 1.81  2000/03/01 02:09:51  robertj
 * Fixed problem if H245 channel established before H225 connect.
 *
 * Revision 1.80  2000/01/07 08:21:32  robertj
 * Added status functions for connection and tidied up the answer call function
 *
 * Revision 1.79  2000/01/04 01:06:06  robertj
 * Removed redundent code, thanks Dave Harvey.
 *
 * Revision 1.78  2000/01/04 00:14:53  craigs
 * Added extra states to AnswerCall
 *
 * Revision 1.77  1999/12/23 23:02:35  robertj
 * File reorganision for separating RTP from H.323 and creation of LID for VPB support.
 *
 * Revision 1.76  1999/12/11 02:21:00  robertj
 * Added ability to have multiple aliases on local endpoint.
 *
 * Revision 1.75  1999/12/09 20:30:54  robertj
 * Fixed problem with receiving multiple fast start open fields in multiple PDU's.
 *
 * Revision 1.74  1999/11/23 03:38:51  robertj
 * Fixed yet another call termination reason code error.
 *
 * Revision 1.73  1999/11/22 10:07:23  robertj
 * Fixed some errors in correct termination states.
 *
 * Revision 1.72  1999/11/20 04:36:56  robertj
 * Fixed setting of transmitter channel numbers on receiving fast start.
 *
 * Revision 1.71  1999/11/19 13:00:25  robertj
 * Added call token into traces so can tell which connection is being cleaned up.
 *
 * Revision 1.70  1999/11/19 08:16:08  craigs
 * Added connectionStartTime
 *
 * Revision 1.69  1999/11/17 04:22:59  robertj
 * Fixed bug in incorrect termination state when making a fast start call.
 *
 * Revision 1.68  1999/11/17 00:01:12  robertj
 * Improved determination of caller name, thanks Ian MacDonald
 *
 * Revision 1.67  1999/11/14 11:25:34  robertj
 * Fixed bug with channel close callback being called twice when fast starting.
 *
 * Revision 1.66  1999/11/13 14:11:24  robertj
 * Fixed incorrect state on fast start receive.
 *
 * Revision 1.65  1999/11/10 23:29:45  robertj
 * Changed OnAnswerCall() call back function  to allow for asyncronous response.
 *
 * Revision 1.64  1999/11/06 11:58:24  robertj
 * Changed clean up to delete logical channels before channel destructor is called.
 *
 * Revision 1.63  1999/11/06 11:03:58  robertj
 * Fixed bug in fast start with H245 channel opening multiple channels.
 * Fixed bug in clean up, write of release complete if TCP connection failed.
 *
 * Revision 1.62  1999/11/06 05:37:45  robertj
 * Complete rewrite of termination of connection to avoid numerous race conditions.
 *
 * Revision 1.61  1999/11/05 08:24:43  robertj
 * Fixed bug in receiver refusing fast start, then not being able to start normally.
 *
 * Revision 1.60  1999/10/30 12:34:46  robertj
 * Added information callback for closed logical channel on H323EndPoint.
 *
 * Revision 1.59  1999/10/29 14:19:14  robertj
 * Fixed incorrect termination code when connection closed locally.
 *
 * Revision 1.58  1999/10/29 02:26:18  robertj
 * Added reason for termination code to H323Connection.
 *
 * Revision 1.57  1999/10/19 00:04:57  robertj
 * Changed OpenAudioChannel and OpenVideoChannel to allow a codec AttachChannel with no autodelete.
 *
 * Revision 1.56  1999/10/16 03:47:49  robertj
 * Fixed termination of gatekeeper RAS thread problem
 *
 * Revision 1.55  1999/10/14 12:05:03  robertj
 * Fixed deadlock possibilities in clearing calls.
 *
 * Revision 1.54  1999/10/10 08:59:47  robertj
 * no message
 *
 * Revision 1.53  1999/10/09 01:18:23  craigs
 * Added codecs to OpenAudioChannel and OpenVideoDevice functions
 *
 * Revision 1.52  1999/10/08 08:31:18  robertj
 * Fixed problem with fast start fall back to capability exchange
 *
 * Revision 1.51  1999/10/07 03:26:58  robertj
 * Fixed some fast-start compatbility problems.
 *
 * Revision 1.50  1999/09/23 07:33:53  robertj
 * Fixed some fast start/245 tunnelling bugs, some odd cases.
 *
 * Revision 1.49  1999/09/23 07:25:12  robertj
 * Added open audio and video function to connection and started multi-frame codec send functionality.
 *
 * Revision 1.48  1999/09/21 14:09:39  robertj
 * Removed warnings when no tracing enabled.
 *
 * Revision 1.47  1999/09/15 01:26:27  robertj
 * Changed capability set call backs to have more specific class as parameter.
 *
 * Revision 1.46  1999/09/14 14:26:17  robertj
 * Added more debug tracing.
 *
 * Revision 1.45  1999/09/10 03:36:48  robertj
 * Added simple Q.931 Status response to Q.931 Status Enquiry
 *
 * Revision 1.44  1999/09/08 04:05:49  robertj
 * Added support for video capabilities & codec, still needs the actual codec itself!
 *
 * Revision 1.43  1999/08/31 12:34:19  robertj
 * Added gatekeeper support.
 *
 * Revision 1.42  1999/08/27 15:42:44  craigs
 * Fixed problem with local call tokens using ambiguous interface names, and connect timeouts not changing connection state
 *
 * Revision 1.41  1999/08/25 05:11:22  robertj
 * File fission (critical mass reached).
 * Improved way in which remote capabilities are created, removed case statement!
 * Changed MakeCall, so immediately spawns thread, no black on TCP connect.
 *
 * Revision 1.40  1999/08/14 03:26:45  robertj
 * Compiler compatibility problems.
 *
 * Revision 1.39  1999/08/10 04:20:26  robertj
 * Fixed constness problems in some PASN_Choice casts.
 *
 * Revision 1.38  1999/08/08 10:03:33  robertj
 * Fixed capability selection to honor local table priority order.
 *
 * Revision 1.37  1999/07/23 02:37:53  robertj
 * Fixed problems with hang ups and crash closes of connections.
 *
 * Revision 1.36  1999/07/19 02:01:02  robertj
 * Fixeed memory leask on connection termination.
 * Fixed connection "orderly close" on endpoint termination.
 *
 * Revision 1.35  1999/07/18 14:57:29  robertj
 * Fixed bugs in slow start with H245 tunnelling, part 4.
 *
 * Revision 1.34  1999/07/18 14:29:31  robertj
 * Fixed bugs in slow start with H245 tunnelling, part 3.
 *
 * Revision 1.33  1999/07/18 13:59:12  robertj
 * Fixed bugs in slow start with H245 tunnelling, part 2.
 *
 * Revision 1.32  1999/07/18 13:25:40  robertj
 * Fixed bugs in slow start with H245 tunnelling.
 *
 * Revision 1.31  1999/07/18 10:19:39  robertj
 * Fixed CreateCapability function: missing break's in case!
 *
 * Revision 1.30  1999/07/17 06:22:32  robertj
 * Fixed bug in setting up control channel when initiating call (recently introduced)
 *
 * Revision 1.29  1999/07/16 16:04:41  robertj
 * Bullet proofed local capability table data entry.
 *
 * Revision 1.28  1999/07/16 14:04:47  robertj
 * Fixed bug that caused signal channel to be closed, forgot to disable connect time.
 *
 * Revision 1.27  1999/07/16 06:15:59  robertj
 * Corrected semantics for tunnelled master/slave determination in fast start.
 *
 * Revision 1.26  1999/07/16 02:15:20  robertj
 * Fixed more tunneling problems.
 * Fixed fastStart initiator matching response to correct channels.
 *
 * Revision 1.25  1999/07/16 00:51:03  robertj
 * Some more debugging of fast start.
 *
 * Revision 1.24  1999/07/15 14:45:36  robertj
 * Added propagation of codec open error to shut down logical channel.
 * Fixed control channel start up bug introduced with tunnelling.
 *
 * Revision 1.23  1999/07/15 09:04:31  robertj
 * Fixed some fast start bugs
 *
 * Revision 1.22  1999/07/14 06:06:14  robertj
 * Fixed termination problems (race conditions) with deleting connection object.
 *
 * Revision 1.21  1999/07/13 09:53:24  robertj
 * Fixed some problems with jitter buffer and added more debugging.
 *
 * Revision 1.20  1999/07/13 02:50:58  craigs
 * Changed semantics of SetPlayDevice/SetRecordDevice, only descendent
 *    endpoint assumes PSoundChannel devices for audio codec.
 *
 * Revision 1.19  1999/07/10 02:59:26  robertj
 * Fixed ability to hang up incoming connection.
 *
 * Revision 1.18  1999/07/10 02:51:36  robertj
 * Added mutexing in H245 procedures. Also fixed MSD state bug.
 *
 * Revision 1.17  1999/07/09 14:59:59  robertj
 * Fixed GNU C++ compatibility.
 *
 * Revision 1.16  1999/07/09 06:09:49  robertj
 * Major implementation. An ENORMOUS amount of stuff added everywhere.
 *
 * Revision 1.15  1999/06/25 10:25:35  robertj
 * Added maintentance of callIdentifier variable in H.225 channel.
 *
 * Revision 1.14  1999/06/22 13:45:05  robertj
 * Added user question on listener version to accept incoming calls.
 *
 * Revision 1.13  1999/06/14 06:39:08  robertj
 * Fixed problem with getting transmit flag to channel from PDU negotiator
 *
 * Revision 1.12  1999/06/14 05:15:55  robertj
 * Changes for using RTP sessions correctly in H323 Logical Channel context
 *
 * Revision 1.11  1999/06/13 12:41:14  robertj
 * Implement logical channel transmitter.
 * Fixed H245 connect on receiving call.
 *
 * Revision 1.10  1999/06/09 06:18:00  robertj
 * GCC compatibiltiy.
 *
 * Revision 1.9  1999/06/09 05:26:19  robertj
 * Major restructuring of classes.
 *
 * Revision 1.8  1999/06/07 00:37:05  robertj
 * Allowed for reuseaddr on listen when in debug version
 *
 * Revision 1.7  1999/06/06 06:06:36  robertj
 * Changes for new ASN compiler and v2 protocol ASN files.
 *
 * Revision 1.6  1999/04/26 06:20:22  robertj
 * Fixed bugs in protocol
 *
 * Revision 1.5  1999/04/26 06:14:46  craigs
 * Initial implementation for RTP decoding and lots of stuff
 * As a whole, these changes are called "First Noise"
 *
 * Revision 1.4  1999/02/23 11:04:28  robertj
 * Added capability to make outgoing call.
 *
 * Revision 1.3  1999/01/16 01:31:33  robertj
 * Major implementation.
 *
 * Revision 1.2  1999/01/02 04:00:56  robertj
 * Added higher level protocol negotiations.
 *
 * Revision 1.1  1998/12/14 09:13:09  robertj
 * Initial revision
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "h323con.h"
#endif

#include <h323/h323con.h>

#include <h323/h323ep.h>
#include <h323/h323neg.h>
#include <h323/h323rtp.h>
#include <h323/gkclient.h>
#include <h323/h450pdu.h>
#include <h323/transaddr.h>
#include <asn/h4504.h>


#define new PNEW


#if !PTRACING // Stuff to remove unised parameters warning
#define PTRACE_pdu
#endif


/////////////////////////////////////////////////////////////////////////////

#if PTRACING
const char * const H323Connection::ConnectionStatesNames[NumConnectionStates] = {
  "NoConnectionActive",
  "AwaitingGatekeeperAdmission",
  "AwaitingTransportConnect",
  "AwaitingSignalConnect",
  "HasExecutedSignalConnect",
  "EstablishedConnection",
  "ShuttingDownConnection"
};

const char * const H323Connection::FastStartStateNames[NumFastStartStates] = {
  "FastStartDisabled",
  "FastStartInitiate",
  "FastStartResponse",
  "FastStartAcknowledged"
};
#endif


H323Connection::H323Connection(OpalCall & call,
                               H323EndPoint & ep,
                               const PString & token,
                               BOOL disableFastStart,
                               BOOL disableTunneling)
  : OpalConnection(call, ep, token),
    endpoint(ep),
    localPartyName(ep.GetLocalUserName()),
    localCapabilities(ep.GetCapabilities())
{
  callAnswered = FALSE;
  distinctiveRing = 0;
  callReference = UINT_MAX;

  signallingChannel = NULL;
  controlChannel = NULL;
  controlListener = NULL;
  h245Tunneling = !disableTunneling;
  h245TunnelPDU = NULL;

  connectionState = NoConnectionActive;

  bandwidthAvailable = endpoint.GetInitialBandwidth();

  nextInvokeId = 0;
  ctInvokeId = -1;
  ctState = e_ctIdle;
  holdState = e_ch_Idle;

  transmitterSidePaused = FALSE;

  fastStartState = disableFastStart ? FastStartDisabled : FastStartInitiate;
  fastStartedTransmitMediaStream = NULL;
  earlyStart = FALSE;
  startT120 = TRUE;

  remoteMaxAudioDelayJitter = 0;
  maxAudioDelayJitter = ep.GetManager().GetMaxAudioDelayJitter();

  masterSlaveDeterminationProcedure = new H245NegMasterSlaveDetermination(endpoint, *this);
  capabilityExchangeProcedure = new H245NegTerminalCapabilitySet(endpoint, *this);
  logicalChannels = new H245NegLogicalChannels(endpoint, *this);
  requestModeProcedure = new H245NegRequestMode(endpoint, *this);
  roundTripDelayProcedure = new H245NegRoundTripDelay(endpoint, *this);

  endSync = NULL;
}


H323Connection::~H323Connection()
{
  delete masterSlaveDeterminationProcedure;
  delete capabilityExchangeProcedure;
  delete logicalChannels;
  delete requestModeProcedure;
  delete roundTripDelayProcedure;
  delete signallingChannel;
  delete controlChannel;

  PTRACE(3, "H323\tConnection " << callToken << " deleted.");

  if (endSync != NULL)
    endSync->Signal();
}


OpalConnection::Phases H323Connection::GetPhase() const
{
  static Phases mapping[] = {
    SetUpPhase,
    SetUpPhase,
    SetUpPhase,
    AlertingPhase,
    ConnectedPhase,
    EstablishedPhase,
    ReleasedPhase
  };
  return mapping[connectionState];
}


BOOL H323Connection::OnReleased()
{
  CleanUpOnCallEnd();
  return OpalConnection::OnReleased();
}


void H323Connection::CleanUpOnCallEnd()
{
  PTRACE(3, "H323\tConnection " << callToken << " closing: connectionState=" << connectionState);

  // A kludge to avoid a deadlock, grab the lock, set the connectionState to
  // indicate we are shutting down then release the lock with a short sleep
  // to assure all threads waiting on that lock have time to get scheduled.
  // When they are they see the connection state and exit immediately.

  inUseFlag.Wait();
  connectionState = ShuttingDownConnection;
  inUseFlag.Signal();
  PThread::Current()->Sleep(1);
  inUseFlag.Wait();

//  if (connectionState == AwaitingTransportConnect)
//    signallingChannel->CleanUpOnTermination();

  // Clean up any fast start "pending" channels we may have running.
  PINDEX i;
  for (i = 0; i < fastStartChannels.GetSize(); i++)
    fastStartChannels[i].CleanUpOnTermination();
  fastStartChannels.RemoveAll();

  // Dispose of all the logical channels
  logicalChannels->RemoveAll();

  if (callEndReason == EndedByLocalUser || callEndReason == EndedByCallForwarded) {
    // Send an H.245 end session to the remote endpoint.
    H323ControlPDU pdu;
    pdu.BuildEndSessionCommand(H245_EndSessionCommand::e_disconnect);
    WriteControlPDU(pdu);
  }

  /* If we have a signalling channel, then we should send a release complete.
     This may actually be redundent if we a terminating via the remote end
     sending release complete or if there is a transport failure, but it does
     not hurt to try and send it anyway.
   */
  if (signallingChannel != NULL) {
    if (signallingChannel->IsOpen()) {
      PTRACE(2, "H225\tSending release complete PDU: callRef=" << callReference);
      H323SignalPDU replyPDU;
      replyPDU.BuildReleaseComplete(*this);
      replyPDU.Write(*signallingChannel);
    }
    signallingChannel->CloseWait();
  }

  // Wait for control channel to be cleaned up (thread ended).
  if (controlChannel != NULL)
    controlChannel->CloseWait();

  // Check for gatekeeper and do disengage if have one
  if (callEndReason != EndedByGatekeeper) {
    H323Gatekeeper * gatekeeper = endpoint.GetGatekeeper();
    if (gatekeeper != NULL)
      gatekeeper->DisengageRequest(*this, H225_DisengageReason::e_normalDrop);
  }

  PTRACE(1, "H323\tConnection " << callToken << " terminated.");
}


PString H323Connection::GetDestinationAddress()
{
  if (!localDestinationAddress)
    return localDestinationAddress;

  return OpalConnection::GetDestinationAddress();
}


void H323Connection::AttachSignalChannel(OpalTransport * channel, BOOL answeringCall)
{
  callAnswered = answeringCall;

  if (signallingChannel != NULL && signallingChannel->IsOpen()) {
    PAssertAlways(PLogicError);
    return;
  }

  delete signallingChannel;
  signallingChannel = channel;
}


BOOL H323Connection::WriteSignalPDU(H323SignalPDU & pdu)
{
  PAssert(signallingChannel != NULL, PLogicError);
  if (signallingChannel == NULL)
    return FALSE;

  if (!signallingChannel->IsOpen())
    return FALSE;

  pdu.m_h323_uu_pdu.m_h245Tunneling = h245Tunneling;

  return pdu.Write(*signallingChannel);
}


void H323Connection::HandleSignallingChannel()
{
  PAssert(signallingChannel != NULL, PLogicError);

  PTRACE(2, "H225\tReading PDUs: callRef=" << callReference);

  H323SignalPDU pdu;
  while (signallingChannel->IsOpen()) {
    if (pdu.Read(*signallingChannel)) {
      if (!HandleSignalPDU(pdu)) {
        Release(EndedByTransportFail);
        break;
      }
    }
    else if (connectionState != EstablishedConnection ||
             signallingChannel->GetErrorCode() != PChannel::Timeout) {
      if (controlChannel == NULL || !controlChannel->IsOpen())
        Release(signallingChannel->GetErrorCode() != PChannel::Timeout
                                  ? EndedByTransportFail : EndedByNoAnswer);
      signallingChannel->Close();
      break;
    }

    if (controlChannel == NULL)
      StartRoundTripDelay();
  }

  PTRACE(2, "H225\tSignal channel closed.");
}


BOOL H323Connection::HandleSignalPDU(H323SignalPDU & pdu)
{
  if (!Lock())
    return FALSE;

  PWaitAndSignal lockedConnection(inUseFlag, FALSE);

  // Process the PDU.
  const Q931 & q931 = pdu.GetQ931();

  PTRACE(3, "H225\tHandling PDU: " << q931.GetMessageTypeName()
                    << " callRef=" << q931.GetCallReference());

  // If remote does not do tunneling, so we don't either. Note that if it
  // gets turned off once, it stays off for good.
  if (h245Tunneling && !pdu.m_h323_uu_pdu.m_h245Tunneling) {
    masterSlaveDeterminationProcedure->Stop();
    capabilityExchangeProcedure->Stop();
    h245Tunneling = FALSE;
  }
  
  // Check for presence of supplementary services
  if (pdu.m_h323_uu_pdu.HasOptionalField(H225_H323_UU_PDU::e_h4501SupplementaryService))
  {
    // Process H4501SupplementaryService APDU
    HandleSupplementaryServicePDU(pdu);
  }

  switch (q931.GetMessageType()) {
    case Q931::SetupMsg :
      if (!OnReceivedSignalSetup(pdu))
        return FALSE;
      break;

    case Q931::CallProceedingMsg :
      if (!OnReceivedCallProceeding(pdu))
        return FALSE;
      break;

    case Q931::ProgressMsg :
      if (!OnReceivedProgress(pdu))
        return FALSE;
      break;

    case Q931::AlertingMsg :
      if (!OnReceivedAlerting(pdu))
        return FALSE;
      break;

    case Q931::ConnectMsg :
      if (!OnReceivedSignalConnect(pdu))
        return FALSE;
      break;

    case Q931::FacilityMsg :
      if (!OnReceivedFacility(pdu))
        return FALSE;
      break;

    case Q931::StatusEnquiryMsg :
      if (!OnReceivedStatusEnquiry(pdu))
        return FALSE;
      break;

    case Q931::ReleaseCompleteMsg :
      OnReceivedReleaseComplete(pdu);
      return FALSE;

    default :
      if (!OnUnknownSignalPDU(pdu))
        return FALSE;
  }

  // Process tunnelled H245 PDU, if present.
  HandleTunnelPDU(pdu, NULL);

  // Check for establishment criteria met
  InternalEstablishedConnectionCheck();
  return TRUE;
}


void H323Connection::HandleTunnelPDU(const H323SignalPDU & pdu, H323SignalPDU * reply)
{
  if (!h245Tunneling)
    return;

  h245TunnelPDU = reply;

  for (PINDEX i = 0; i < pdu.m_h323_uu_pdu.m_h245Control.GetSize(); i++) {
    PPER_Stream strm = pdu.m_h323_uu_pdu.m_h245Control[i].GetValue();
    HandleControlData(strm);
  }

  h245TunnelPDU = NULL;

  // Make sure does not get repeated, break const and clear tunnelled H.245 PDU's
  ((H323SignalPDU &)pdu).m_h323_uu_pdu.m_h245Control.SetSize(0);
}


void H323Connection::HandleSupplementaryServicePDU(const H323SignalPDU & pdu)
{
  for (PINDEX i = 0; i < pdu.m_h323_uu_pdu.m_h4501SupplementaryService.GetSize(); i++) {
    H4501_SupplementaryService supplementaryService;

    // Decode the supplementary service PDU from the PPER Stream
    if (pdu.m_h323_uu_pdu.m_h4501SupplementaryService[i].DecodeSubType(supplementaryService)) {
      PTRACE(4, "H4501\tSupplementary service PDU:\n  "
             << setprecision(2) << supplementaryService);
    }
    else {
      PTRACE(1, "H4501\tInvalid supplementary service PDU decode:\n  "
             << setprecision(2) << supplementaryService);
      continue;
    }

    if (supplementaryService.m_serviceApdu.GetTag() == H4501_ServiceApdus::e_rosApdus) {
      H4501_ArrayOf_ROS& operations = (H4501_ArrayOf_ROS&) supplementaryService.m_serviceApdu;

      for (PINDEX j = 0; j < operations.GetSize(); j ++) {
        X880_ROS& operation = operations[j];

        PTRACE(3, "H4501\tX880 ROS " << operation.GetTagName());

        switch (operation.GetTag()) {
          case X880_ROS::e_invoke:
            OnReceivedInvoke((X880_Invoke&) operation);
            break;

          case X880_ROS::e_returnResult:
            OnReceivedReturnResult((X880_ReturnResult&) operation);
            break;

          case X880_ROS::e_returnError:
            OnReceivedReturnError((X880_ReturnError&) operation);
            break;

          case X880_ROS::e_reject:
            OnReceivedReject((X880_Reject&) operation);
            break;

          default :
            break;
        }
      }
    }
  }
}


void H323Connection::OnReceivedInvoke(X880_Invoke & invoke)
{
  // Get the invokeId
  int invokeId = invoke.m_invokeId.GetValue();

  // Get the linkedId if present
  int linkedId = -1;
  if (invoke.HasOptionalField(X880_Invoke::e_linkedId)) {
    linkedId = invoke.m_linkedId.GetValue();
  }

  // Get the argument if present
  PASN_OctetString * argument = NULL;
  if (invoke.HasOptionalField(X880_Invoke::e_argument)) {
    argument = &invoke.m_argument;
  }

  // Get the opcode
  if (invoke.m_opcode.GetTag() == X880_Code::e_local) {
    int opcode = ((PASN_Integer&) invoke.m_opcode).GetValue();

    switch (opcode) {
      case H4502_CallTransferOperation::e_callTransferIdentify:
        OnReceivedCallTransferIdentify(invokeId, linkedId);
        break;

      case H4502_CallTransferOperation::e_callTransferAbandon:
        OnReceivedCallTransferAbandon(invokeId, linkedId);
        break;

      case H4502_CallTransferOperation::e_callTransferInitiate:
        OnReceivedCallTransferInitiate(invokeId, linkedId, argument);
        break;

      case H4502_CallTransferOperation::e_callTransferSetup:
        OnReceivedCallTransferSetup(invokeId, linkedId, argument);
        break;

      case H4502_CallTransferOperation::e_callTransferUpdate:
        OnReceivedCallTransferUpdate(invokeId, linkedId, argument);
        break;

      case H4502_CallTransferOperation::e_subaddressTransfer:
        OnReceivedSubaddressTransfer(invokeId, linkedId, argument);
        break;

      case H4502_CallTransferOperation::e_callTransferComplete:
        OnReceivedCallTransferComplete(invokeId, linkedId, argument);
        break;

      case H4502_CallTransferOperation::e_callTransferActive:
        OnReceivedCallTransferActive(invokeId, linkedId, argument);
        break;

      case H4504_CallHoldOperation::e_holdNotific:
	OnReceivedLocalCallHold(invokeId, linkedId);
	break;

      case H4504_CallHoldOperation::e_retrieveNotific:
	OnReceivedLocalCallRetrieve(invokeId, linkedId);
	break;

      case H4504_CallHoldOperation::e_remoteHold:
	OnReceivedRemoteCallHold(invokeId, linkedId);
	break;

      case H4504_CallHoldOperation::e_remoteRetrieve:
	OnReceivedRemoteCallRetrieve(invokeId, linkedId);
	break;

      default:
        PTRACE(2, "H4501\tInvoke of unsupported local opcode:\n  " << invoke);
        SendInvokeReject(invokeId, 1 /*X880_InvokeProblem::e_unrecognisedOperation*/);
        break;
    }
  }
  else {
    SendInvokeReject(invokeId, 1 /*X880_InvokeProblem::e_unrecognisedOperation*/);
    PTRACE(2, "H4501\tInvoke of unsupported global opcode:\n  " << invoke);
  }
}


void H323Connection::OnReceivedReturnResult(X880_ReturnResult & returnResult)
{
  int invokeId = returnResult.m_invokeId.GetValue();

  if (invokeId == GetCallTransferInvokeId()) {
    switch (GetCallTransferState()) {
      default :
        break;

      case e_ctAwaitInitiateResponse:
        // stop timer CT-T3
        // clear the primary call, if it exists
        SetCallTransferState(e_ctIdle);
        endpoint.OnCallTransferInitiateReturnResult(*this);
        break;

      case e_ctAwaitSetupResponse:
        // stop timer CT-T4

        // Clear the call
        SetCallTransferState(e_ctIdle);
        endpoint.OnCallTransferSetupReturnResult(*this);
        endpoint.ClearCall(GetTransferringToken(), EndedByCallForwarded);
        break;
    }
  }
}


void H323Connection::OnReceivedReturnError(X880_ReturnError & returnError)
{
  int invokeId = returnError.m_invokeId.GetValue();
  int errorCode = 0;

  if (returnError.m_errorCode.GetTag() == X880_Code::e_local)
    errorCode = ((PASN_Integer&) returnError.m_errorCode).GetValue();

  if (invokeId == GetCallTransferInvokeId()) {
    switch (GetCallTransferState()) {
      default :
        break;

      case e_ctAwaitInitiateResponse:
        // stop timer CT-T3
        // clear the primary call, if it exists
        SetCallTransferState(e_ctIdle);
        endpoint.OnCallTransferInitiateReturnError(*this, errorCode);
        break;

      case H323Connection::e_ctAwaitSetupResponse:
        // stop timer CT-T4

        // Send a facility to the transferring endpoint
        // containing a call transfer initiate return error
        H323Connection * existingConnection = (H323Connection *)endpoint.FindConnectionWithLock(GetTransferringToken());

        if (existingConnection != NULL)
        {
          H323SignalPDU facility;
          H450ServiceAPDU serviceAPDU;

          facility.BuildFacility(*existingConnection, TRUE);
          serviceAPDU.BuildReturnError(existingConnection->GetCallTransferInvokeId(), errorCode);

          facility.AttachSupplementaryServiceAPDU(serviceAPDU);
          existingConnection->WriteSignalPDU(facility);

          existingConnection->Unlock();
        }
        SetCallTransferState(e_ctIdle);
        endpoint.OnCallTransferSetupReturnError(*this, errorCode);
        break;
    }
  }
}


void H323Connection::OnReceivedReject(X880_Reject & reject)
{
  int invokeId = reject.m_invokeId;
  int problem = 0;

  switch (reject.m_problem.GetTag()) {
    case X880_Reject_problem::e_general:
    {
      X880_GeneralProblem& generalProblem = (X880_GeneralProblem&) reject.m_problem;
      problem = generalProblem.GetValue();
    }
    break;

    case X880_Reject_problem::e_invoke:
    {
      X880_InvokeProblem& invokeProblem = (X880_InvokeProblem&) reject.m_problem;
      problem = invokeProblem.GetValue();
    }
    break;

    case X880_Reject_problem::e_returnResult:
    {
      X880_ReturnResultProblem& returnResultProblem = (X880_ReturnResultProblem&) reject.m_problem;
      problem = returnResultProblem.GetValue();
    }
    break;

    case X880_Reject_problem::e_returnError:
    {
      X880_ReturnErrorProblem& returnErrorProblem = (X880_ReturnErrorProblem&) reject.m_problem;
      problem = returnErrorProblem.GetValue();
    }
    break;

    default:
      break;
  }

  endpoint.OnReject(*this, invokeId, problem);
}


void H323Connection::OnReceivedCallTransferIdentify(int /*invokeId*/,
                                                    int /*linkedId*/)
{
}


void H323Connection::OnReceivedCallTransferAbandon(int /*invokeId*/,
                                                   int /*linkedId*/)
{
}


static BOOL H4501ArgumentDecode(PASN_OctetString * argString, PASN_Object & argObject)
{
  if (argString == NULL)
    return FALSE;

  PPER_Stream argStream(*argString);
  if (argObject.Decode(argStream)) {
    PTRACE(4, "H4501\tSupplementary service argument:\n  "
           << setprecision(2) << argObject);
    return TRUE;
  }

  PTRACE(1, "H4501\tInvalid supplementary service argument:\n  "
         << setprecision(2) << argObject);
  return FALSE;
}


void H323Connection::OnReceivedCallTransferInitiate(int invokeId,
                                                    int /*linkedId*/,
                                                    PASN_OctetString * argument)
{
  // TBD: Check Call Hold status. If call is held, it must first be 
  // retrieved before being transferred. -- dcassel 4/01


  if (argument != NULL) {
    H4502_CTInitiateArg ctInitiateArg;
    if (H4501ArgumentDecode(argument, ctInitiateArg)) {
      PString remoteParty;
      H450ServiceAPDU::ParseEndpointAddress(ctInitiateArg.m_reroutingNumber, remoteParty);

      if (endpoint.OnCallTransferInitiate(*this, remoteParty)) {
        PString token;
        endpoint.SetupTransfer(GetToken(), ctInitiateArg.m_callIdentity.GetValue(), remoteParty, token);
        ctInvokeId = invokeId;
      }
      else {
        SendReturnError(invokeId, H4502_CallTransferErrors::e_establishmentFailure);
      }
    }
  }
  else {
    SendReturnError(invokeId, H4502_CallTransferErrors::e_invalidReroutingNumber);
  }
}


void H323Connection::OnReceivedCallTransferSetup(int invokeId,
                                                 int linkedId,
                                                 PASN_OctetString * argument)
{
  if (argument != NULL) {
    H4502_CTSetupArg ctSetupArg;
    if (H4501ArgumentDecode(argument, ctSetupArg)) {
      if (endpoint.OnCallTransferSetup(*this, invokeId, linkedId, ctSetupArg.m_callIdentity.GetValue())) {
        SetCallTransferState(e_ctAwaitSetupResponse);
        ctInvokeId = invokeId;
      }
      else {
        SendReturnError(invokeId, H4502_CallTransferErrors::e_unrecognizedCallIdentity);
      }
    }
  }
  else {
    SendReturnError(invokeId, H4502_CallTransferErrors::e_unrecognizedCallIdentity);
  }
}


void H323Connection::OnReceivedCallTransferUpdate(int /*invokeId*/,
                                                  int /*linkedId*/,
                                                  PASN_OctetString * argument)
{
  H4502_CTUpdateArg ctUpdateArg;
  if (H4501ArgumentDecode(argument, ctUpdateArg)) {
    //
  }
}


void H323Connection::OnReceivedSubaddressTransfer(int /*invokeId*/,
                                                  int /*linkedId*/,
                                                  PASN_OctetString * argument)
{
  H4502_SubaddressTransferArg subaddressTransferArg;
  if (H4501ArgumentDecode(argument, subaddressTransferArg)) {
    //
  }
}


void H323Connection::OnReceivedCallTransferComplete(int /*invokeId*/,
                                                    int /*linkedId*/,
                                                    PASN_OctetString * argument)
{
  H4502_CTCompleteArg ctCompleteArg;
  if (H4501ArgumentDecode(argument, ctCompleteArg)) {
    //
  }
}


void H323Connection::OnReceivedCallTransferActive(int /*invokeId*/,
                                                  int /*linkedId*/,
                                                  PASN_OctetString * argument)
{
  H4502_CTActiveArg ctActiveArg;
  if (H4501ArgumentDecode(argument, ctActiveArg)) {
    //
  }
}


void H323Connection::OnReceivedLocalCallHold(int /*invokeId*/, int /*linkedId*/)
{
  // Pause channels
  for (PINDEX i = 0; i < logicalChannels->GetSize(); i++) {
    H323Channel * channel = logicalChannels->GetChannelAt(i);
    if (NULL != channel)
      channel->SetPause(true);
  }
}


void H323Connection::OnReceivedLocalCallRetrieve(int /*invokeId*/, int /*linkedId*/)
{
  // Reactivate channels
  for (PINDEX i = 0; i < logicalChannels->GetSize(); i++) {
    H323Channel * channel = logicalChannels->GetChannelAt(i);
    if (NULL != channel)
      channel->SetPause(false);
  }
}


void H323Connection::OnReceivedRemoteCallHold(int /*invokeId*/, int /*linkedId*/)
{
	// TBD
}


void H323Connection::OnReceivedRemoteCallRetrieve(int /*invokeId*/, int /*linkedId*/)
{
	// TBD
}


void H323Connection::SendReturnError(int invokeId, int returnError)
{
  H323SignalPDU facilityPDU;
  facilityPDU.BuildFacility(*this, TRUE);

  H450ServiceAPDU serviceAPDU;

  serviceAPDU.BuildReturnError(invokeId, returnError);
  facilityPDU.AttachSupplementaryServiceAPDU(serviceAPDU);

  WriteSignalPDU(facilityPDU);
}


void H323Connection::SendGeneralReject(int invokeId, int problem)
{
  H323SignalPDU facilityPDU;
  facilityPDU.BuildFacility(*this, TRUE);

  H450ServiceAPDU serviceAPDU;

  X880_Reject & reject = serviceAPDU.BuildReject(invokeId);
  reject.m_problem.SetTag(X880_Reject_problem::e_general);
  X880_GeneralProblem & generalProblem = (X880_GeneralProblem &) reject.m_problem;
  generalProblem = problem;

  facilityPDU.AttachSupplementaryServiceAPDU(serviceAPDU);
  WriteSignalPDU(facilityPDU);
}


void H323Connection::SendInvokeReject(int invokeId, int problem)
{
  H323SignalPDU facilityPDU;
  facilityPDU.BuildFacility(*this, TRUE);

  H450ServiceAPDU serviceAPDU;

  X880_Reject & reject = serviceAPDU.BuildReject(invokeId);
  reject.m_problem.SetTag(X880_Reject_problem::e_invoke);
  X880_InvokeProblem & invokeProblem = (X880_InvokeProblem &) reject.m_problem;
  invokeProblem = problem;

  facilityPDU.AttachSupplementaryServiceAPDU(serviceAPDU);
  WriteSignalPDU(facilityPDU);
}


void H323Connection::SendReturnResultReject(int invokeId, int problem)
{
  H323SignalPDU facilityPDU;
  facilityPDU.BuildFacility(*this, TRUE);

  H450ServiceAPDU serviceAPDU;

  X880_Reject & reject = serviceAPDU.BuildReject(invokeId);
  reject.m_problem.SetTag(X880_Reject_problem::e_returnResult);
  X880_ReturnResultProblem & returnResultProblem = (X880_ReturnResultProblem &) reject.m_problem;
  returnResultProblem = problem;

  facilityPDU.AttachSupplementaryServiceAPDU(serviceAPDU);
  WriteSignalPDU(facilityPDU);
}


void H323Connection::SendReturnErrorReject(int invokeId, int problem)
{
  H323SignalPDU facilityPDU;
  facilityPDU.BuildFacility(*this, TRUE);

  H450ServiceAPDU serviceAPDU;

  X880_Reject & reject = serviceAPDU.BuildReject(invokeId);
  reject.m_problem.SetTag(X880_Reject_problem::e_returnError);
  X880_ReturnErrorProblem & returnErrorProblem = (X880_ReturnErrorProblem &) reject.m_problem;
  returnErrorProblem = problem;

  facilityPDU.AttachSupplementaryServiceAPDU(serviceAPDU);
  WriteSignalPDU(facilityPDU);
}


static BOOL BuildFastStartList(const H323Channel & channel,
                               H225_ArrayOf_PASN_OctetString & array,
                               H323Channel::Directions reverseDirection)
{
  H245_OpenLogicalChannel open;
  const H323Capability & capability = channel.GetCapability();

  if (channel.GetDirection() != reverseDirection) {
    if (!capability.OnSendingPDU(open.m_forwardLogicalChannelParameters.m_dataType))
      return FALSE;
  }
  else {
    if (!capability.OnSendingPDU(open.m_reverseLogicalChannelParameters.m_dataType))
      return FALSE;

    open.m_forwardLogicalChannelParameters.m_multiplexParameters.SetTag(
                H245_OpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters::e_none);
    open.m_forwardLogicalChannelParameters.m_dataType.SetTag(H245_DataType::e_nullData);
    open.IncludeOptionalField(H245_OpenLogicalChannel::e_reverseLogicalChannelParameters);
  }

  if (!channel.OnSendingPDU(open))
    return FALSE;

  PTRACE(4, "H225\tBuild fastStart:\n  " << setprecision(2) << open);
  PINDEX last = array.GetSize();
  array.SetSize(last+1);
  array[last].EncodeSubType(open);

  PTRACE(3, "H225\tBuilt fastStart for " << capability);
  return TRUE;
}

void H323Connection::OnEstablished()
{
  endpoint.OnConnectionEstablished(*this, callToken);
}

void H323Connection::OnCleared()
{
  endpoint.OnConnectionCleared(*this, callToken);
}


BOOL H323Connection::OnReceivedSignalSetup(const H323SignalPDU & setupPDU)
{
  PINDEX i;

  if (setupPDU.m_h323_uu_pdu.m_h323_message_body.GetTag() != H225_H323_UU_PDU_h323_message_body::e_setup)
    return FALSE;
  const H225_Setup_UUIE & setup = setupPDU.m_h323_uu_pdu.m_h323_message_body;

  // Get the ring pattern
  distinctiveRing = setupPDU.GetDistinctiveRing();

  // Save the identifiers sent by caller
  if (setup.HasOptionalField(H225_Setup_UUIE::e_callIdentifier))
    callIdentifier = setup.m_callIdentifier.m_guid;
  conferenceIdentifier = setup.m_conferenceID;
  SetRemoteApplication(setup.m_sourceInfo);

  // Determine the remote parties name/number/address as best we can
  setupPDU.GetQ931().GetCallingPartyNumber(remotePartyNumber);
  remotePartyName = setupPDU.GetSourceAliases(signallingChannel);

  if (setup.m_sourceAddress.GetSize() > 0)
    remotePartyAddress = H323GetAliasAddressString(setup.m_sourceAddress[0]) + '@' + signallingChannel->GetRemoteAddress();
  else
    remotePartyAddress = signallingChannel->GetRemoteAddress();

  // Send back a H323 Call Proceeding PDU in case OnIncomingCall() takes a while
  PTRACE(3, "H225\tSending call proceeding PDU");
  H323SignalPDU callProceedingPDU;
  callProceedingPDU.BuildCallProceeding(*this);
  if (OnSendCallProceeding(callProceedingPDU)) {
    if (!WriteSignalPDU(callProceedingPDU))
      return FALSE;
  }

  // if the application indicates not to contine, then send a Q931 Release Complete PDU
  H323SignalPDU alertingPDU;
  H225_Alerting_UUIE & alerting = alertingPDU.BuildAlerting(*this);

  if (!OnIncomingCall(setupPDU, alertingPDU)) {
    Release(EndedByNoAccept);
    PTRACE(1, "H225\tApplication not accepting calls");
    return FALSE;
  }

  // send Q931 Alerting PDU
  PTRACE(3, "H225\tIncoming call accepted");

  // Check for gatekeeper and do admission check if have one
  H323Gatekeeper * gatekeeper = endpoint.GetGatekeeper();
  if (gatekeeper != NULL) {
    if (!gatekeeper->AdmissionRequest(*this)) {
      unsigned reason = gatekeeper->GetRejectReason();
      PTRACE(1, "H225\tGatekeeper refused admission: "
             << H225_AdmissionRejectReason(reason).GetTagName());
      switch (reason) {
        case H225_AdmissionRejectReason::e_calledPartyNotRegistered :
          Release(EndedByNoUser);
        case H225_AdmissionRejectReason::e_requestDenied :
          Release(EndedByNoBandwidth);
        default :
          Release(EndedByGatekeeper);
      }
      return FALSE;
    }
  }

  // Get the local capabilities before fast start is handled
  OnSetLocalCapabilities();

  // See if remote endpoint wants to start fast
  if ((fastStartState != FastStartDisabled) && setup.HasOptionalField(H225_Setup_UUIE::e_fastStart)) {
    remoteCapabilities.RemoveAll();
    PTRACE(3, "H225\tFast start detected");

    // Extract capabilities from the fast start OpenLogicalChannel structures
    for (i = 0; i < setup.m_fastStart.GetSize(); i++) {
      H245_OpenLogicalChannel open;
      if (setup.m_fastStart[i].DecodeSubType(open)) {
        PTRACE(4, "H225\tFast start open:\n  " << setprecision(2) << open);
        unsigned error;
        H323Channel * channel = CreateLogicalChannel(open, TRUE, error);
        if (channel != NULL) {
          if (channel->GetDirection() == H323Channel::IsTransmitter)
            channel->SetNumber(logicalChannels->GetNextChannelNumber());
          fastStartChannels.Append(channel);
        }
      }
      else {
        PTRACE(1, "H225\tInvalid fast start PDU decode:\n  " << open);
      }
    }

    PTRACE(3, "H225\tOpened " << fastStartChannels.GetSize() << " fast start channels");

    // If we are incapable of ANY of the fast start channels, don't do fast start
    if (!fastStartChannels.IsEmpty())
      fastStartState = FastStartResponse;
  }

  // Build the reply with the channels we are actually using
  H323SignalPDU connectPDU;
  H225_Connect_UUIE & connect = connectPDU.BuildConnect(*this);

  // call the application callback to determine if to answer the call or not
  AnswerCallResponse response = OnAnswerCall(remotePartyName, setupPDU, connectPDU);
  if (response != NumAnswerCallResponses)
    answerResponse = response;

  PTRACE(3, "H225\tAnswer call response: " << answerResponse);

  BOOL ctResponseSent = FALSE;

  // If pending response wait
  while ((answerResponse != AnswerCallNow) && (answerResponse != AnswerCallDenied)) {

    // send alerting PDU if required
    switch (answerResponse) {
      case AnswerCallPending:
      case AnswerCallAlertWithMedia :
        // send Q931 Alerting PDU
        PTRACE(3, "H225\tSending Alerting PDU");
        HandleTunnelPDU(setupPDU, &alertingPDU);

        if (!setup.m_mediaWaitForConnect && answerResponse == AnswerCallAlertWithMedia) {
          if (SendFastStartAcknowledge(alerting.m_fastStart))
            alerting.IncludeOptionalField(H225_Alerting_UUIE::e_fastStart);
          else {
            // Do early H.245 start
            earlyStart = TRUE;
            if (!h245Tunneling && controlChannel == NULL) {
              if (!CreateIncomingControlChannel(alerting.m_h245Address))
                return FALSE;

              alerting.IncludeOptionalField(H225_Alerting_UUIE::e_h245Address);
            }
          }
        }

        // Do we need to send a callTransferSetup return result APDU?
        if ((ctInvokeId != -1) && (ctResponseSent == FALSE))
        {
          H450ServiceAPDU serviceAPDU;

          serviceAPDU.BuildReturnResult(ctInvokeId);
          alertingPDU.AttachSupplementaryServiceAPDU(serviceAPDU);
          ctResponseSent = TRUE;
        }

        if (!WriteSignalPDU(alertingPDU))
          return FALSE;

        answerResponse = AnswerCallDeferred;
        break;

      case AnswerCallDeferredWithMedia :
        if (!setup.m_mediaWaitForConnect) {
          H323SignalPDU want245PDU;
          H225_Progress_UUIE & prog = want245PDU.BuildProgress(*this);

          BOOL sendPDU = TRUE;

          if (SendFastStartAcknowledge(prog.m_fastStart))
            prog.IncludeOptionalField(H225_Progress_UUIE::e_fastStart);
          else {
            // Do early H.245 start
            H225_Facility_UUIE & fac = *want245PDU.BuildFacility(*this, FALSE);
            fac.m_reason.SetTag(H225_FacilityReason::e_startH245);
            earlyStart = TRUE;
            if (!h245Tunneling && (controlChannel == NULL)) {
              if (!CreateIncomingControlChannel(fac.m_h245Address))
                return FALSE;

              fac.IncludeOptionalField(H225_Facility_UUIE::e_h245Address);
            } else
              sendPDU = FALSE;
          }

          if (sendPDU) {

            // Do we need to send a callTransferSetup return result APDU?
            if ((ctInvokeId != -1) && (ctResponseSent == FALSE))
            {
              H450ServiceAPDU serviceAPDU;

              serviceAPDU.BuildReturnResult(ctInvokeId);
              want245PDU.AttachSupplementaryServiceAPDU(serviceAPDU);
              ctResponseSent = TRUE;
            }

            if (!WriteSignalPDU(want245PDU))
              return FALSE;
          }

          answerResponse = AnswerCallDeferred;
        }
    }

    // Wait for answer from application, but check for call being cleared in
    // the mean time. Need to unlock ourselves here as can deadlock waiting
    // for application to do a FindConnectionWithLock() to set the answer
    // call state variable in AnswerCall().
    inUseFlag.Signal();

    // Wait for answer
    BOOL answerWaitTimedOut = !answerWaitFlag.Wait(500);

    // Lock while checking for shutting down.
    if (!Lock())
      return FALSE;

    // Need to check the TCP connection ever now and then to see if the caller
    // has aborted the call or simply disconnected the TCP link (ie crashed).
    if (answerWaitTimedOut) {
      PTimeInterval oldTimeout = signallingChannel->GetReadTimeout();
      signallingChannel->SetReadTimeout(0);

      H323SignalPDU pdu;
      if (pdu.Read(*signallingChannel)) {
        inUseFlag.Signal(); // Prevent recursive locking in HandleSignalPDE()
        if (!HandleSignalPDU(pdu))
          return FALSE;
        if (!Lock())
          return FALSE;
      }
      else {
        if (signallingChannel->GetErrorCode() != PChannel::Timeout)
          return FALSE;
      }

      signallingChannel->SetReadTimeout(oldTimeout);
    }
  }
  PTRACE(3, "H225\tAnswer call response: " << answerResponse);

  // If response is denied, abort the call
  if (answerResponse == AnswerCallDenied) {
    PTRACE(1, "H225\tApplication has declined to answer incoming call");

    // If the SETUP message we received from the other end had a callTransferSetup APDU
    // in it, then we need to send back a RELEASE COMPLETE PDU with a callTransferSetup 
    // ReturnError.
    // Else normal call - clear it down
    if ((ctInvokeId != -1) && (ctResponseSent == FALSE))
    {
      H323SignalPDU releaseCompletePDU;
      releaseCompletePDU.BuildReleaseComplete(*this);

      H450ServiceAPDU serviceAPDU;

      serviceAPDU.BuildReturnError(ctInvokeId, H4501_GeneralErrorList::e_notAvailable);
      releaseCompletePDU.AttachSupplementaryServiceAPDU(serviceAPDU);
      ctResponseSent = TRUE;

      WriteSignalPDU(releaseCompletePDU);
    }
    else
    {
      Release(EndedByAnswerDenied);
    }
    return FALSE;
  }

  // Now ask the application to select which channels to start
  if (SendFastStartAcknowledge(connect.m_fastStart))
    connect.IncludeOptionalField(H225_Connect_UUIE::e_fastStart);

  // Set flag that we are up to CONNECT stage
  if (connectionState == ShuttingDownConnection)
    return FALSE;
  connectionState = HasExecutedSignalConnect;

  if ((ctInvokeId != -1) && (ctResponseSent == FALSE))
  {
    H450ServiceAPDU serviceAPDU;

    serviceAPDU.BuildReturnResult(ctInvokeId);
    connectPDU.AttachSupplementaryServiceAPDU(serviceAPDU);
    ctResponseSent = TRUE;
  }

  // Start separate H.245 channel if not tunneling.
  if (!h245Tunneling && controlChannel == NULL) {
    if (!CreateIncomingControlChannel(connect.m_h245Address))
      return FALSE;

    connect.IncludeOptionalField(H225_Connect_UUIE::e_h245Address);
  }

 
  if (h245Tunneling) {
    // If no channels selected (or never provided) do traditional H245 start
    if (fastStartState == FastStartDisabled) {
      h245TunnelPDU = &connectPDU; // Piggy back H245 on this reply
      if (!StartControlNegotiations())
        return FALSE;
      HandleTunnelPDU(setupPDU, &connectPDU);
    }
  }
  else if (controlChannel == NULL) { // Start separate H.245 channel if not tunneling.
    if (!CreateIncomingControlChannel(connect.m_h245Address))
      return FALSE;

    connect.IncludeOptionalField(H225_Connect_UUIE::e_h245Address);
  }

  return WriteSignalPDU(connectPDU); // Send H323 Connect PDU
}


void H323Connection::SetRemotePartyInfo(const H323SignalPDU & pdu)
{
  PString newNumber;
  if (pdu.GetQ931().GetCalledPartyNumber(newNumber))
    remotePartyNumber = newNumber;

  PString remoteHostName = signallingChannel->GetRemoteAddress().GetHostName();

  PString newRemotePartyName = pdu.GetQ931().GetDisplayName();
  if (newRemotePartyName.IsEmpty() || newRemotePartyName == remoteHostName)
    remotePartyName = remoteHostName;
  else
    remotePartyName = newRemotePartyName + " [" + remoteHostName + ']';

  PTRACE(2, "H225\tSet remote party name: \"" << remotePartyName << '"');
}


void H323Connection::SetRemoteApplication(const H225_EndpointType & pdu)
{
  PStringStream str;

  if (pdu.HasOptionalField(H225_EndpointType::e_vendor))
    str << pdu.m_vendor.m_productId.AsString() << '\t'
        << pdu.m_vendor.m_versionId.AsString() << '\t'
        << pdu.m_vendor.m_vendor.m_t35CountryCode << ':'
        << pdu.m_vendor.m_vendor.m_t35Extension << ':'
        << pdu.m_vendor.m_vendor.m_manufacturerCode;

  remoteApplication = str;
  PTRACE(2, "H225\tSet remote application name: \"" << remoteApplication << '"');

  // Hack to fix Altigen problem. They do stupid things if there is an
  // receiveUserInputCapability entry in the capabilityTable!
  if (remoteApplication.Find("AltiServ-ITG") != P_MAX_INDEX) {
    H323Capability * cap;
    while ((cap = localCapabilities.FindCapability(H323Capability::e_UserInput, UINT_MAX)) != NULL)
      localCapabilities.Remove(cap);
  }
}


BOOL H323Connection::OnReceivedCallProceeding(const H323SignalPDU & pdu)
{
  if (pdu.m_h323_uu_pdu.m_h323_message_body.GetTag() != H225_H323_UU_PDU_h323_message_body::e_callProceeding)
    return FALSE;
  const H225_CallProceeding_UUIE & call = pdu.m_h323_uu_pdu.m_h323_message_body;

  SetRemotePartyInfo(pdu);
  SetRemoteApplication(call.m_destinationInfo);

  // Check for fastStart data and start fast
  if (call.HasOptionalField(H225_CallProceeding_UUIE::e_fastStart))
    HandleFastStartAcknowledge(call.m_fastStart);

  // Check that it has the H.245 channel connection info
  if (call.HasOptionalField(H225_CallProceeding_UUIE::e_h245Address))
    return CreateOutgoingControlChannel(call.m_h245Address);

  return TRUE;
}


BOOL H323Connection::OnReceivedProgress(const H323SignalPDU & pdu)
{
  if (pdu.m_h323_uu_pdu.m_h323_message_body.GetTag() != H225_H323_UU_PDU_h323_message_body::e_progress)
    return FALSE;
  const H225_Progress_UUIE & progress = pdu.m_h323_uu_pdu.m_h323_message_body;

  SetRemotePartyInfo(pdu);
  SetRemoteApplication(progress.m_destinationInfo);

  // Check for fastStart data and start fast
  if (progress.HasOptionalField(H225_Progress_UUIE::e_fastStart))
    HandleFastStartAcknowledge(progress.m_fastStart);

  // Check that it has the H.245 channel connection info
  if (progress.HasOptionalField(H225_Progress_UUIE::e_h245Address))
    return CreateOutgoingControlChannel(progress.m_h245Address);

  return TRUE;
}


BOOL H323Connection::OnReceivedAlerting(const H323SignalPDU & pdu)
{
  if (pdu.m_h323_uu_pdu.m_h323_message_body.GetTag() != H225_H323_UU_PDU_h323_message_body::e_alerting)
    return FALSE;
  const H225_Alerting_UUIE & alert = pdu.m_h323_uu_pdu.m_h323_message_body;

  SetRemotePartyInfo(pdu);
  SetRemoteApplication(alert.m_destinationInfo);

  // Check for fastStart data and start fast
  if (alert.HasOptionalField(H225_Alerting_UUIE::e_fastStart))
    HandleFastStartAcknowledge(alert.m_fastStart);

  // Check that it has the H.245 channel connection info
  if (alert.HasOptionalField(H225_Alerting_UUIE::e_h245Address))
    if (!CreateOutgoingControlChannel(alert.m_h245Address))
      return FALSE;

  return OnAlerting(pdu, remotePartyName);
}


BOOL H323Connection::OnReceivedSignalConnect(const H323SignalPDU & pdu)
{
  if (connectionState == ShuttingDownConnection)
    return FALSE;
  connectionState = HasExecutedSignalConnect;

  if (pdu.m_h323_uu_pdu.m_h323_message_body.GetTag() != H225_H323_UU_PDU_h323_message_body::e_connect)
    return FALSE;
  const H225_Connect_UUIE & connect = pdu.m_h323_uu_pdu.m_h323_message_body;

  SetRemotePartyInfo(pdu);
  SetRemoteApplication(connect.m_destinationInfo);

  if (!OnOutgoingCall(pdu)) {
    Release(EndedByNoAccept);
    return FALSE;
  }

  // have answer, so set timeout to interval for pinging remote
  signallingChannel->SetReadTimeout(endpoint.GetRoundTripDelayRate());

  // Check for fastStart data and start fast
  if (connect.HasOptionalField(H225_Connect_UUIE::e_fastStart))
    HandleFastStartAcknowledge(connect.m_fastStart);

  // Check that it has the H.245 channel connection info
  if (connect.HasOptionalField(H225_Connect_UUIE::e_h245Address)) {
    if (!CreateOutgoingControlChannel(connect.m_h245Address)) {
      if (fastStartState != FastStartAcknowledged)
        return FALSE;
    }
  }

  // If didn't get fast start channels accepted by remote then clear our
  // proposed channels
  if (fastStartState != FastStartAcknowledged) {
    fastStartState = FastStartDisabled;
    fastStartChannels.RemoveAll();
  }

  OnConnected();

  // If we have a H.245 channel available, bring it up. We either have media
  // and this is just so user indications work, or we don't have media and
  // desperately need it!
  if (h245Tunneling || controlChannel != NULL)
    return StartControlNegotiations();

  // We have no tunnelling and not separate channel, but we really want one
  // so we will start one using a facility message
  PTRACE(2, "H225\tNo H245 address provided by remote, starting control channel");

  H323SignalPDU want245PDU;
  H225_Facility_UUIE * fac = want245PDU.BuildFacility(*this, FALSE);

  fac->m_reason.SetTag(H225_FacilityReason::e_startH245);
  fac->IncludeOptionalField(H225_Facility_UUIE::e_h245Address);

  if (!CreateIncomingControlChannel(fac->m_h245Address))
    return FALSE;

  return WriteSignalPDU(want245PDU);
}


BOOL H323Connection::OnReceivedFacility(const H323SignalPDU & pdu)
{
  if (pdu.m_h323_uu_pdu.m_h323_message_body.GetTag() == H225_H323_UU_PDU_h323_message_body::e_empty)
    return TRUE;

  if (pdu.m_h323_uu_pdu.m_h323_message_body.GetTag() != H225_H323_UU_PDU_h323_message_body::e_facility)
    return FALSE;
  const H225_Facility_UUIE & fac = pdu.m_h323_uu_pdu.m_h323_message_body;

  // Check for fastStart data and start fast
  if (fac.HasOptionalField(H225_Facility_UUIE::e_fastStart))
    HandleFastStartAcknowledge(fac.m_fastStart);

  // Check that it has the H.245 channel connection info
  if (fac.HasOptionalField(H225_Facility_UUIE::e_h245Address))
    return CreateOutgoingControlChannel(fac.m_h245Address);

  if (fac.m_reason.GetTag() != H225_FacilityReason::e_callForwarded)
    return TRUE;

  PString address;
  if (fac.HasOptionalField(H225_Facility_UUIE::e_alternativeAliasAddress) &&
      fac.m_alternativeAliasAddress.GetSize() > 0)
    address = H323GetAliasAddressString(fac.m_alternativeAliasAddress[0]);

  if (fac.HasOptionalField(H225_Facility_UUIE::e_alternativeAddress)) {
    if (!address)
      address += '@';
    address += H323TransportAddress(fac.m_alternativeAddress);
  }

  if (endpoint.OnConnectionForwarded(*this, address, pdu)) {
    Release(EndedByCallForwarded);
    return FALSE;
  }

  return TRUE;
}


BOOL H323Connection::OnReceivedStatusEnquiry(const H323SignalPDU & /*pdu*/)
{
  H323SignalPDU reply;
  reply.BuildStatus(*this);
  return reply.Write(*signallingChannel);
}


void H323Connection::OnReceivedReleaseComplete(const H323SignalPDU & pdu)
{
  if (connectionState == EstablishedConnection)
    Release(EndedByRemoteUser);
  else if (answerResponse == AnswerCallDeferred)
    Release(EndedByCallerAbort);
  else {
    if (callEndReason == EndedByRefusal)
      callEndReason = NumCallEndReasons;
    switch (pdu.GetQ931().GetCause()) {
      case Q931::UserBusy :
        Release(EndedByRemoteBusy);
        break;
      case Q931::Congestion :
        Release(EndedByRemoteCongestion);
        break;
      case Q931::NoRouteToDestination :
        Release(EndedByNoUser);
        break;
      case Q931::Redirection :
        Release(EndedByCallForwarded);
        break;
      case Q931::NoResponse :
      case Q931::NoAnswer :
        Release(EndedByNoAnswer);
        break;
      default :
        Release(EndedByRefusal);
    }
  }
}


BOOL H323Connection::OnIncomingCall(const H323SignalPDU & setupPDU,
                                    H323SignalPDU & alertingPDU)
{
  return endpoint.OnIncomingCall(*this, setupPDU, alertingPDU);
}


BOOL H323Connection::ForwardCall(const PString & forwardParty)
{
  if (forwardParty.IsEmpty())
    return FALSE;

  PString alias;
  H323TransportAddress address;
  endpoint.ParsePartyName(forwardParty, alias, address);

  H323SignalPDU redirectPDU;
  H225_Facility_UUIE * fac = redirectPDU.BuildFacility(*this, FALSE);

  fac->m_reason.SetTag(H225_FacilityReason::e_callForwarded);

  if (!address) {
    fac->IncludeOptionalField(H225_Facility_UUIE::e_alternativeAddress);
    address.SetPDU(fac->m_alternativeAddress);
  }

  if (!alias) {
    fac->IncludeOptionalField(H225_Facility_UUIE::e_alternativeAliasAddress);
    fac->m_alternativeAliasAddress.SetSize(1);
    H323SetAliasAddress(alias, fac->m_alternativeAliasAddress[0]);
  }

  if (WriteSignalPDU(redirectPDU))
    Release(EndedByCallForwarded);

  return TRUE;
}


H323Connection::AnswerCallResponse
     H323Connection::OnAnswerCall(const PString & caller,
                                  const H323SignalPDU & setupPDU,
                                  H323SignalPDU & connectPDU)
{
  return endpoint.OnAnswerCall(*this, caller, setupPDU, connectPDU);
}


void H323Connection::AnsweringCall(AnswerCallResponse response)
{
  SetAnswerResponse(response);
}


H323Connection::CallEndReason H323Connection::SendSignalSetup(const PString & alias,
                                                              const H323TransportAddress & address)
{
  if (!Lock())
    return EndedByLocalUser;

  PWaitAndSignal lockedConnection(inUseFlag, FALSE);

  // Start the call, first state is asking gatekeeper
  connectionState = AwaitingGatekeeperAdmission;

  // Indicate the direction of call.
  if (alias.IsEmpty())
    remotePartyName = remotePartyAddress = address;
  else {
    remotePartyName = alias;
    remotePartyAddress = alias + '@' + address;
  }

  // Start building the setup PDU to get various ID's
  H323SignalPDU setupPDU;
  H225_Setup_UUIE & setup = setupPDU.BuildSetup(*this, address);

  // Do we need to attach a call transfer setup invoke APDU?
  if (GetCallTransferState() == e_ctAwaitSetupResponse) {
    H450ServiceAPDU serviceAPDU;

    // Store the outstanding invokeID associated with this connection
    ctInvokeId = GetNextInvokeId();

    // Use the call identity from the ctInitiateArg
    serviceAPDU.BuildCallTransferSetup(GetCallTransferInvokeId(), GetTransferringCallIdentity());

    setupPDU.AttachSupplementaryServiceAPDU(serviceAPDU);
  }

  // Save the identifiers generated by BuildSetup
  conferenceIdentifier = setup.m_conferenceID;
  setupPDU.GetQ931().GetCalledPartyNumber(remotePartyNumber);

  H323TransportAddress gatekeeperRoute = address;

  // Check for gatekeeper and do admission check if have one
  H323Gatekeeper * gatekeeper = endpoint.GetGatekeeper();
  H225_ArrayOf_AliasAddress newAliasAddress;
  if (gatekeeper != NULL) {
    if (!gatekeeper->AdmissionRequest(*this, gatekeeperRoute, newAliasAddress, alias.IsEmpty())) {
      unsigned reason = gatekeeper->GetRejectReason();
      PTRACE(1, "H225\tGatekeeper refused admission: "
             << H225_AdmissionRejectReason(reason).GetTagName());
      switch (reason) {
        case H225_AdmissionRejectReason::e_calledPartyNotRegistered :
          return EndedByNoUser;
        case H225_AdmissionRejectReason::e_requestDenied :
          return EndedByNoBandwidth;
      }
      return EndedByGatekeeper;
    }
  }

  // Update the field e_destinationAddress in the SETUP PDU to reflect the new 
  // alias received in the ACF (m_destinationInfo).
  if (newAliasAddress.GetSize() > 0) {
    setup.IncludeOptionalField(H225_Setup_UUIE::e_destinationAddress);
    setup.m_destinationAddress = newAliasAddress;
  }

  if (!signallingChannel->SetRemoteAddress(gatekeeperRoute)) {
    PTRACE(1, "H225\tInvalid "
           << (gatekeeperRoute != address ? "gatekeeper" : "user")
           << " supplied address: \"" << gatekeeperRoute << '"');
    connectionState = AwaitingTransportConnect;
    return EndedByConnectFail;
  }

  // Do the transport connect
  connectionState = AwaitingTransportConnect;

  // Release the mutex as can deadlock trying to clear call during connect.
  inUseFlag.Signal();

  BOOL connectFailed = !signallingChannel->Connect();

    // Lock while checking for shutting down.
  if (!Lock())
    return EndedByConnectFail;

  // See if transport connect failed, abort if so.
  if (connectFailed) {
    connectionState = NoConnectionActive;
    switch (signallingChannel->GetErrorNumber()) {
      case ENETUNREACH :
        return EndedByUnreachable;
      case ECONNREFUSED :
        return EndedByNoEndPoint;
      case ETIMEDOUT :
        return EndedByHostOffline;
    }
    return EndedByConnectFail;
  }

  PTRACE(3, "H225\tSending Setup PDU");
  connectionState = AwaitingSignalConnect;

  // Put in all the signalling addresses for link
  H323TransportAddress transportAddress = signallingChannel->GetLocalAddress();
  setup.IncludeOptionalField(H225_Setup_UUIE::e_sourceCallSignalAddress);
  transportAddress.SetPDU(setup.m_sourceCallSignalAddress);
  if (!setup.HasOptionalField(H225_Setup_UUIE::e_destCallSignalAddress)) {
    transportAddress = signallingChannel->GetRemoteAddress();
    setup.IncludeOptionalField(H225_Setup_UUIE::e_destCallSignalAddress);
    transportAddress.SetPDU(setup.m_destCallSignalAddress);
  }

  // Get the local capabilities before fast start is handled
  OnSetLocalCapabilities();

  // Ask the application what channels to open
  PTRACE(3, "H225\tCheck for Fast start by local endpoint");
  fastStartChannels.RemoveAll();
  OnSelectLogicalChannels();

  // If application called OpenLogicalChannel, put in the fastStart field
  if (!fastStartChannels.IsEmpty()) {
    PTRACE(3, "H225\tFast start begun by local endpoint");
    for (PINDEX i = 0; i < fastStartChannels.GetSize(); i++)
      BuildFastStartList(fastStartChannels[i], setup.m_fastStart, H323Channel::IsReceiver);
    if (setup.m_fastStart.GetSize() > 0)
      setup.IncludeOptionalField(H225_Setup_UUIE::e_fastStart);
  }

  if (!OnSendSignalSetup(setupPDU))
    return EndedByNoAccept;

  // Do this again (was done when PDU was constructed) in case
  // OnSendSignalSetup() changed something.
  setupPDU.SetQ931Fields(*this, TRUE);
  setupPDU.GetQ931().GetCalledPartyNumber(remotePartyNumber);

  fastStartState = FastStartDisabled;

  if (h245Tunneling) {
    h245TunnelPDU = &setupPDU;

    // Cannot do capability exchange if if fast starting though.
    if (fastStartChannels.IsEmpty()) {
      // Try and start the master/slave and capability exchange through the tunnel
      if (!StartControlNegotiations())
        return EndedByTransportFail;
    }

    h245TunnelPDU = NULL;
  }

  // Send the initial PDU
  if (!WriteSignalPDU(setupPDU))
    return EndedByTransportFail;

  // Set timeout for remote party to answer the call
  signallingChannel->SetReadTimeout(endpoint.GetSignallingChannelCallTimeout());

  return NumCallEndReasons;
}


BOOL H323Connection::OnSendSignalSetup(H323SignalPDU & /*setupPDU*/)
{
  return TRUE;
}


BOOL H323Connection::OnSendCallProceeding(H323SignalPDU & /*callProceedingPDU*/)
{
  return TRUE;
}


BOOL H323Connection::OnAlerting(const H323SignalPDU & alertingPDU,
                                const PString & username)
{
  return endpoint.OnAlerting(*this, alertingPDU, username);
}


BOOL H323Connection::SetAlerting(const PString & /*calleeName*/)
{
  PTRACE(3, "H323\tSetAlerting " << *this);
  SetAnswerResponse(AnswerCallPending);
  return TRUE;
}


BOOL H323Connection::SetConnected()
{
  PTRACE(3, "H323\tSetConnected " << *this);
  SetAnswerResponse(AnswerCallNow);
  return TRUE;
}


BOOL H323Connection::OnOutgoingCall(const H323SignalPDU & /*connectPDU*/)
{
  PTRACE(1, "H225\tReceived connect PDU.");
  return TRUE;
}


BOOL H323Connection::SendFastStartAcknowledge(H225_ArrayOf_PASN_OctetString & array)
{
  PINDEX i;

  if (fastStartState == FastStartResponse)
    OnSelectLogicalChannels();

  // Remove any channels that were not started by OnSelectLogicalChannels(),
  // those that were started are put into the logical channel dictionary
  for (i = 0; i < fastStartChannels.GetSize(); i++) {
    if (fastStartChannels[i].IsRunning())
      logicalChannels->Add(fastStartChannels[i]);
    else
      fastStartChannels.RemoveAt(i--);
  }

  // None left, so didn't open any channels fast
  if (fastStartChannels.IsEmpty()) {
    fastStartState = FastStartDisabled;
    return FALSE;
  }

  // The channels we just transferred to the logical channels dictionary
  // should not be deleted via this structure now.
  fastStartChannels.DisallowDeleteObjects();

  PTRACE(3, "H225\tAccepting fastStart for " << fastStartChannels.GetSize() << " channels");

  for (i = 0; i < fastStartChannels.GetSize(); i++)
    BuildFastStartList(fastStartChannels[i], array, H323Channel::IsTransmitter);

  // Have moved open channels to logicalChannels structure, remove all others.
  fastStartChannels.RemoveAll();

  // Set flag so internal establishment check does not require H.245
  fastStartState = FastStartAcknowledged;

  return TRUE;
}


BOOL H323Connection::HandleFastStartAcknowledge(const H225_ArrayOf_PASN_OctetString & array)
{
  if (fastStartChannels.IsEmpty()) {
    PTRACE(3, "H225\tFast start response with no channels to open");
    return FALSE;
  }

  PTRACE(3, "H225\tFast start accepted by remote endpoint");

  PINDEX i;

  // Go through provided list of structures, if can decode it and match it up
  // with a channel we requested AND it has all the information needed in the
  // m_multiplexParameters, then we can start the channel.
  for (i = 0; i < array.GetSize(); i++) {
    H245_OpenLogicalChannel open;
    if (array[i].DecodeSubType(open)) {
      PTRACE(4, "H225\tFast start open:\n  " << setprecision(2) << open);
      BOOL reverse = open.HasOptionalField(H245_OpenLogicalChannel::e_reverseLogicalChannelParameters);
      const H245_DataType & dataType = reverse ? open.m_reverseLogicalChannelParameters.m_dataType
                                               : open.m_forwardLogicalChannelParameters.m_dataType;
      H323Capability * replyCapability = localCapabilities.FindCapability(dataType);
      if (replyCapability != NULL) {
        for (PINDEX ch = 0; ch < fastStartChannels.GetSize(); ch++) {
          H323Channel & channelToStart = fastStartChannels[ch];
          if ((channelToStart.GetDirection() == H323Channel::IsReceiver) == reverse) {
            if (channelToStart.GetCapability() == *replyCapability) {
              H323Capability * remoteCapability = replyCapability;
              unsigned error = 1000;
              if (channelToStart.OnReceivedPDU(open, error)) {
                // For transmitter, need to fake a capability into the remote table
                if (channelToStart.GetDirection() == H323Channel::IsTransmitter) {
                  remoteCapability = remoteCapabilities.Copy(*replyCapability);
                  remoteCapabilities.SetCapability(0, replyCapability->GetDefaultSessionID()-1, remoteCapability);
                }
                if (OnCreateLogicalChannel(*remoteCapability, channelToStart.GetDirection(), error)) {
                  if (channelToStart.SetInitialBandwidth()) {
                    if (channelToStart.GetDirection() == H323Channel::IsTransmitter) {
                      if (channelToStart.Open()) {
                        fastStartedTransmitMediaStream = ((H323UnidirectionalChannel &)channelToStart).GetMediaStream();
                        if (GetCall().OpenSourceMediaStreams(*this, fastStartedTransmitMediaStream->GetMediaFormats(), channelToStart.GetSessionID()))
                          channelToStart.Start();
                        else
                          fastStartedTransmitMediaStream = NULL;
                      }
                    }
                      channelToStart.Start();
                    break;
                  }
                  else
                    PTRACE(2, "H225\tFast start channel open fail: insufficent bandwidth");
                }
                else
                  PTRACE(2, "H225\tFast start channel open error: " << error);
              }
              else
                PTRACE(2, "H225\tFast start capability error: " << error);
            }
          }
        }
      }
    }
    else {
      PTRACE(1, "H225\tInvalid fast start PDU decode:\n  " << setprecision(2) << open);
    }
  }

  // Remove any channels that were not started by above, those that were
  // started are put into the logical channel dictionary
  for (i = 0; i < fastStartChannels.GetSize(); i++) {
    if (fastStartChannels[i].IsRunning())
      logicalChannels->Add(fastStartChannels[i]);
    else
      fastStartChannels.RemoveAt(i--);
  }

  // The channels we just transferred to the logical channels dictionary
  // should not be deleted via this structure now.
  fastStartChannels.DisallowDeleteObjects();

  PTRACE(2, "H225\tFast starting " << fastStartChannels.GetSize() << " channels");
  if (fastStartChannels.IsEmpty())
    return FALSE;

  // Have moved open channels to logicalChannels structure, remove them now.
  fastStartChannels.RemoveAll();

  fastStartState = FastStartAcknowledged;
  return TRUE;
}


BOOL H323Connection::OnUnknownSignalPDU(const H323SignalPDU & PTRACE_pdu)
{
  PTRACE(2, "H225\tUnknown signalling PDU: " << PTRACE_pdu);
  return TRUE;
}


BOOL H323Connection::CreateOutgoingControlChannel(const H225_TransportAddress & h245Address)
{
  // Already have the H245 channel up.
  if (controlChannel != NULL)
    return TRUE;

  // Check that it is an IP address, all we support at the moment
  controlChannel = signallingChannel->GetLocalAddress().CreateTransport(endpoint);
  if (controlChannel == NULL) {
    PTRACE(1, "H225\tConnect of H245 failed: Unsupported transport");
    return FALSE;
  }

  if (!controlChannel->SetRemoteAddress(H323TransportAddress(h245Address))) {
    PTRACE(1, "H225\tCould not extract H245 address");
    delete controlChannel;
    controlChannel = NULL;
    return FALSE;
  }

  if (!controlChannel->Connect()) {
    PTRACE(1, "H225\tConnect of H245 failed: " << controlChannel->GetErrorText());
    delete controlChannel;
    controlChannel = NULL;
    return FALSE;
  }

  return TRUE;
}


BOOL H323Connection::CreateIncomingControlChannel(H225_TransportAddress & h245Address)
{
  PAssert(controlChannel == NULL, PLogicError);

  OpalTransportAddress localSignallingInterface = signallingChannel->GetLocalAddress();
  if (controlListener == NULL) {
    controlListener = localSignallingInterface.CreateCompatibleListener(endpoint);
    if (controlListener == NULL)
      return FALSE;

    if (!controlListener->Open(PCREATE_NOTIFIER(NewIncomingControlChannel), TRUE)) {
      delete controlListener;
      controlListener = NULL;
      return FALSE;
    }
  }

  H323TransportAddress listeningAddress = controlListener->GetLocalAddress(localSignallingInterface);
  return listeningAddress.SetPDU(h245Address);
}


void H323Connection::NewIncomingControlChannel(PThread &, INT param)
{
  // If H.245 channel failed to connect and have no media (no fast start)
  // then clear the call as it is useless.
  if (param == 0 &&
      FindChannel(OpalMediaFormat::DefaultAudioSessionID, TRUE) == NULL ||
      FindChannel(OpalMediaFormat::DefaultAudioSessionID, FALSE) == NULL) {
    ClearCall(EndedByTransportFail);
    return;
  }

  controlChannel = (OpalTransport *)param;
  HandleControlChannel();
}


BOOL H323Connection::WriteControlPDU(const H323ControlPDU & pdu)
{
  PPER_Stream strm;
  pdu.Encode(strm);
  strm.CompleteEncoding();

#if PTRACING
  if (PTrace::CanTrace(4))
    PTRACE(4, "H245\tSending PDU\n  " << setprecision(2) << pdu
                            << "\n "  << setprecision(2) << strm);
  else
    PTRACE(3, "H245\tSending PDU: " << pdu.GetTagName()
           << ' ' << ((PASN_Choice &)pdu.GetObject()).GetTagName());
#endif

  if (!h245Tunneling) {
    if (controlChannel == NULL) {
      PTRACE(1, "H245\tWrite PDU fail: no control channel.");
      return FALSE;
    }

    if (controlChannel->IsOpen() && controlChannel->WritePDU(strm))
      return TRUE;

    PTRACE(1, "H245\tWrite PDU fail: " << controlChannel->GetErrorText());
    return FALSE;
  }

  // If have a pending signalling PDU, use it rather than separate write
  H323SignalPDU localTunnelPDU;
  H323SignalPDU * tunnelPDU;
  if (h245TunnelPDU != NULL)
    tunnelPDU = h245TunnelPDU;
  else {
    localTunnelPDU.BuildFacility(*this, TRUE);
    tunnelPDU = &localTunnelPDU;
  }

  tunnelPDU->m_h323_uu_pdu.IncludeOptionalField(H225_H323_UU_PDU::e_h245Control);
  PINDEX last = tunnelPDU->m_h323_uu_pdu.m_h245Control.GetSize();
  tunnelPDU->m_h323_uu_pdu.m_h245Control.SetSize(last+1);
  tunnelPDU->m_h323_uu_pdu.m_h245Control[last] = strm;

  if (h245TunnelPDU != NULL)
    return TRUE;

  return WriteSignalPDU(localTunnelPDU);
}


BOOL H323Connection::StartControlNegotiations()
{
  PTRACE(2, "H245\tStarted control channel");

  // Begin the capability exchange procedure
  if (!capabilityExchangeProcedure->Start(FALSE)) {
    PTRACE(1, "H245\tStart of Capability Exchange failed");
    return FALSE;
  }

  // Begin the Master/Slave determination procedure
  if (!masterSlaveDeterminationProcedure->Start(FALSE)) {
    PTRACE(1, "H245\tStart of Master/Slave determination failed");
    return FALSE;
  }

  return TRUE;
}


void H323Connection::HandleControlChannel()
{
  // If have started separate H.245 channel then don't tunnel any more
  h245Tunneling = FALSE;

  // Start the TCS and MSD operations on new H.245 channel.
  if (!StartControlNegotiations())
    return;

  controlChannel->SetReadTimeout(endpoint.GetRoundTripDelayRate());

  BOOL ok = TRUE;
  while (ok) {
    StartRoundTripDelay();

    PPER_Stream strm;
    if (controlChannel->ReadPDU(strm)) {
      // Lock while checking for shutting down.
      if (!Lock())
        break;

      // Process the received PDU
      PTRACE(4, "H245\tReceived TPKT: " << strm);
      ok = HandleControlData(strm);

      // Unlock connection
      Unlock();
    }
    else if (connectionState != EstablishedConnection ||
             controlChannel->GetErrorCode() != PChannel::Timeout) {
      PTRACE(1, "H245\tRead error: " << controlChannel->GetErrorText());
      Release(EndedByTransportFail);
      ok = FALSE;
    }
  }

  PTRACE(2, "H245\tControl channel closed.");
}


BOOL H323Connection::HandleControlData(PPER_Stream & strm)
{
  while (!strm.IsAtEnd()) {
    H323ControlPDU pdu;
    if (!pdu.Decode(strm)) {
      PTRACE(1, "H245\tInvalid PDU decode:\n  " << setprecision(2) << pdu);
      return TRUE;
    }

    if (!HandleControlPDU(pdu))
      return FALSE;

    InternalEstablishedConnectionCheck();

    strm.ByteAlign();
  }

  return TRUE;
}


BOOL H323Connection::HandleControlPDU(const H323ControlPDU & pdu)
{
#if PTRACING
  if (PTrace::CanTrace(4))
    PTRACE(4, "H245\tReceived PDU (connectionState=" << connectionState
           << "):\n  " << setprecision(2) << pdu);
  else
    PTRACE(3, "H245\tReceived PDU: " << pdu.GetTagName()
           << ' ' << ((PASN_Choice &)pdu.GetObject()).GetTagName());
#endif

  switch (pdu.GetTag()) {
    case H245_MultimediaSystemControlMessage::e_request :
      return OnH245Request(pdu);

    case H245_MultimediaSystemControlMessage::e_response :
      return OnH245Response(pdu);

    case H245_MultimediaSystemControlMessage::e_command :
      return OnH245Command(pdu);

    case H245_MultimediaSystemControlMessage::e_indication :
      return OnH245Indication(pdu);
  }

  return OnUnknownControlPDU(pdu);
}


BOOL H323Connection::OnUnknownControlPDU(const H323ControlPDU & pdu)
{
  PTRACE(2, "H245\tUnknown Control PDU: " << pdu);

  H323ControlPDU reply;
  reply.BuildFunctionNotUnderstood(pdu);
  return WriteControlPDU(reply);
}


BOOL H323Connection::OnH245Request(const H323ControlPDU & pdu)
{
  const H245_RequestMessage & request = pdu;

  switch (request.GetTag()) {
    case H245_RequestMessage::e_masterSlaveDetermination :
      return masterSlaveDeterminationProcedure->HandleIncoming(request);

    case H245_RequestMessage::e_terminalCapabilitySet :
      return capabilityExchangeProcedure->HandleIncoming(request);

    case H245_RequestMessage::e_openLogicalChannel :
      return logicalChannels->HandleOpen(request);

    case H245_RequestMessage::e_closeLogicalChannel :
      return logicalChannels->HandleClose(request);

    case H245_RequestMessage::e_requestChannelClose :
      return logicalChannels->HandleRequestClose(request);

    case H245_RequestMessage::e_requestMode :
      return requestModeProcedure->HandleRequest(request);

    case H245_RequestMessage::e_roundTripDelayRequest :
      return roundTripDelayProcedure->HandleRequest(request);
  }

  return OnUnknownControlPDU(pdu);
}


BOOL H323Connection::OnH245Response(const H323ControlPDU & pdu)
{
  const H245_ResponseMessage & response = pdu;

  switch (response.GetTag()) {
    case H245_ResponseMessage::e_masterSlaveDeterminationAck :
      return masterSlaveDeterminationProcedure->HandleAck(response);

    case H245_ResponseMessage::e_masterSlaveDeterminationReject :
      return masterSlaveDeterminationProcedure->HandleReject(response);

    case H245_ResponseMessage::e_terminalCapabilitySetAck :
      return capabilityExchangeProcedure->HandleAck(response);

    case H245_ResponseMessage::e_terminalCapabilitySetReject :
      return capabilityExchangeProcedure->HandleReject(response);

    case H245_ResponseMessage::e_openLogicalChannelAck :
      return logicalChannels->HandleOpenAck(response);

    case H245_ResponseMessage::e_openLogicalChannelReject :
      return logicalChannels->HandleReject(response);

    case H245_ResponseMessage::e_closeLogicalChannelAck :
      return logicalChannels->HandleCloseAck(response);

    case H245_ResponseMessage::e_requestChannelCloseAck :
      return logicalChannels->HandleRequestCloseAck(response);

    case H245_ResponseMessage::e_requestChannelCloseReject :
      return logicalChannels->HandleRequestCloseReject(response);

    case H245_ResponseMessage::e_requestModeAck :
      return requestModeProcedure->HandleAck(response);

    case H245_ResponseMessage::e_requestModeReject :
      return requestModeProcedure->HandleReject(response);

    case H245_ResponseMessage::e_roundTripDelayResponse :
      return roundTripDelayProcedure->HandleResponse(response);
  }

  return OnUnknownControlPDU(pdu);
}


BOOL H323Connection::OnH245Command(const H323ControlPDU & pdu)
{
  const H245_CommandMessage & command = pdu;

  switch (command.GetTag()) {
    case H245_CommandMessage::e_sendTerminalCapabilitySet :
      return OnH245_SendTerminalCapabilitySet(command);

    case H245_CommandMessage::e_flowControlCommand :
      return OnH245_FlowControlCommand(command);

    case H245_CommandMessage::e_miscellaneousCommand :
      return OnH245_MiscellaneousCommand(command);

    case H245_CommandMessage::e_endSessionCommand :
      if (connectionState == EstablishedConnection)
        Release(EndedByRemoteUser);
      else if (answerResponse == AnswerCallDeferred)
        Release(EndedByCallerAbort);
      else
        Release(EndedByRefusal);
      return FALSE;
  }

  return OnUnknownControlPDU(pdu);
}


BOOL H323Connection::OnH245Indication(const H323ControlPDU & pdu)
{
  const H245_IndicationMessage & indication = pdu;

  switch (indication.GetTag()) {
    case H245_IndicationMessage::e_masterSlaveDeterminationRelease :
      return masterSlaveDeterminationProcedure->HandleRelease(indication);

    case H245_IndicationMessage::e_terminalCapabilitySetRelease :
      return capabilityExchangeProcedure->HandleRelease(indication);

    case H245_IndicationMessage::e_openLogicalChannelConfirm :
      return logicalChannels->HandleOpenConfirm(indication);

    case H245_IndicationMessage::e_requestChannelCloseRelease :
      return logicalChannels->HandleRequestCloseRelease(indication);

    case H245_IndicationMessage::e_requestModeRelease :
      return requestModeProcedure->HandleRelease(indication);

    case H245_IndicationMessage::e_miscellaneousIndication :
      return OnH245_MiscellaneousIndication(indication);

    case H245_IndicationMessage::e_jitterIndication :
      return OnH245_JitterIndication(indication);

    case H245_IndicationMessage::e_userInput :
      OnUserInputIndication(indication);
      break;
  }

  return TRUE; // Do NOT call OnUnknownControlPDU for indications
}


BOOL H323Connection::OnH245_SendTerminalCapabilitySet(
                 const H245_SendTerminalCapabilitySet & pdu)
{
  if (pdu.GetTag() == H245_SendTerminalCapabilitySet::e_genericRequest)
    return capabilityExchangeProcedure->Start(TRUE);

  PTRACE(2, "H245\tUnhandled SendTerminalCapabilitySet: " << pdu);
  return TRUE;
}


BOOL H323Connection::OnH245_FlowControlCommand(
                 const H245_FlowControlCommand & pdu)
{
  PTRACE(3, "H245\tFlowControlCommand: scope=" << pdu.m_scope.GetTagName());

  long restriction;
  if (pdu.m_restriction.GetTag() == H245_FlowControlCommand_restriction::e_maximumBitRate)
    restriction = (const PASN_Integer &)pdu.m_restriction;
  else
    restriction = -1; // H245_FlowControlCommand_restriction::e_noRestriction

  switch (pdu.m_scope.GetTag()) {
    case H245_FlowControlCommand_scope::e_wholeMultiplex :
      OnLogicalChannelFlowControl(NULL, restriction);
      break;

    case H245_FlowControlCommand_scope::e_logicalChannelNumber :
    {
      H323Channel * chan = logicalChannels->FindChannel((unsigned)(const H245_LogicalChannelNumber &)pdu.m_scope, FALSE);
      if (chan != NULL)
        OnLogicalChannelFlowControl(chan, restriction);
    }
  }

  return TRUE;
}


BOOL H323Connection::OnH245_MiscellaneousCommand(
                 const H245_MiscellaneousCommand & pdu)
{
  H323Channel * chan = logicalChannels->FindChannel((unsigned)pdu.m_logicalChannelNumber, FALSE);
  if (chan != NULL)
    chan->OnMiscellaneousCommand(pdu.m_type);
  else
    PTRACE(3, "H245\tMiscellaneousCommand: chan=" << pdu.m_logicalChannelNumber
           << ", type=" << pdu.m_type.GetTagName());

  return TRUE;
}


BOOL H323Connection::OnH245_MiscellaneousIndication(
                 const H245_MiscellaneousIndication & pdu)
{
  H323Channel * chan = logicalChannels->FindChannel((unsigned)pdu.m_logicalChannelNumber, TRUE);
  if (chan != NULL)
    chan->OnMiscellaneousIndication(pdu.m_type);
  else
    PTRACE(3, "H245\tMiscellaneousIndication: chan=" << pdu.m_logicalChannelNumber
           << ", type=" << pdu.m_type.GetTagName());

  return TRUE;
}


BOOL H323Connection::OnH245_JitterIndication(
                 const H245_JitterIndication & pdu)
{
  PTRACE(3, "H245\tJitterIndication: scope=" << pdu.m_scope.GetTagName());

  static const DWORD mantissas[8] = { 0, 1, 10, 100, 1000, 10000, 100000, 1000000 };
  static const DWORD exponents[8] = { 10, 25, 50, 75 };
  DWORD jitter = mantissas[pdu.m_estimatedReceivedJitterMantissa]*
                 exponents[pdu.m_estimatedReceivedJitterExponent]/10;

  int skippedFrameCount = -1;
  if (pdu.HasOptionalField(H245_JitterIndication::e_skippedFrameCount))
    skippedFrameCount = pdu.m_skippedFrameCount;

  int additionalBuffer = -1;
  if (pdu.HasOptionalField(H245_JitterIndication::e_additionalDecoderBuffer))
    additionalBuffer = pdu.m_additionalDecoderBuffer;

  switch (pdu.m_scope.GetTag()) {
    case H245_JitterIndication_scope::e_wholeMultiplex :
      OnLogicalChannelJitter(NULL, jitter, skippedFrameCount, additionalBuffer);
      break;

    case H245_JitterIndication_scope::e_logicalChannelNumber :
    {
      H323Channel * chan = logicalChannels->FindChannel((unsigned)(const H245_LogicalChannelNumber &)pdu.m_scope, FALSE);
      if (chan != NULL)
        OnLogicalChannelJitter(chan, jitter, skippedFrameCount, additionalBuffer);
    }
  }

  return TRUE;
}


H323Channel * H323Connection::GetLogicalChannel(unsigned number, BOOL fromRemote) const
{
  return logicalChannels->FindChannel(number, fromRemote);
}


H323Channel * H323Connection::FindChannel(unsigned rtpSessionId, BOOL fromRemote) const
{
  return logicalChannels->FindChannelBySession(rtpSessionId, fromRemote);
}


void H323Connection::TransferCall(const PString & remoteParty)
{
  ctInvokeId = GetNextInvokeId();

  // Send a FACILITY message with a callTransferInitiate Invoke
  // Supplementary Service PDU to the transferred endpoint.
  H323SignalPDU facilityPDU;
  H450ServiceAPDU serviceAPDU;

  PString alias;
  OpalTransportAddress address;
  endpoint.ParsePartyName(remoteParty, alias, address);

  PString callIdentity;
  serviceAPDU.BuildCallTransferInitiate(GetCallTransferInvokeId(), callIdentity, alias, address);
  facilityPDU.BuildFacility(*this, TRUE);
  facilityPDU.AttachSupplementaryServiceAPDU(serviceAPDU);
  WriteSignalPDU(facilityPDU);

  // start timer CT-T3
  SetCallTransferState(H323Connection::e_ctAwaitInitiateResponse);
}


void H323Connection::HoldCall(BOOL localHold)
{
  // TBD: Implement Remote Hold. This implementation only does 
  // local hold. -- dcassel 4/01. 
  if (!localHold)
    return;
  
  // Send a FACILITY message with a callNotific Invoke
  // Supplementary Service PDU to the held endpoint.
  H323SignalPDU facilityPDU;
  H450ServiceAPDU serviceAPDU;

  chInvokeId = GetNextInvokeId();
  serviceAPDU.BuildInvoke(chInvokeId, H4504_CallHoldOperation::e_holdNotific);
  facilityPDU.BuildFacility(*this, TRUE);
  facilityPDU.AttachSupplementaryServiceAPDU(serviceAPDU);
  WriteSignalPDU(facilityPDU);
  
  // Pause channels
  for (PINDEX i = 0; i < logicalChannels->GetSize(); i++) {
    H323Channel * channel = logicalChannels->GetChannelAt(i);
    if (NULL != channel)
      channel->SetPause(true);
  }
  
  // Update hold state
  holdState = e_ch_NE_Held;
}


void H323Connection::RetrieveCall(bool localHold)
{
  // TBD: Implement Remote Hold. This implementation only does 
  // local hold. -- dcassel 4/01. 
  if (!localHold)
    return;
  
  // Send a FACILITY message with a retrieveNotific Invoke
  // Supplementary Service PDU to the held endpoint.
  chInvokeId = GetNextInvokeId();
  H323SignalPDU facilityPDU;
  H450ServiceAPDU serviceAPDU;
  
  serviceAPDU.BuildInvoke(chInvokeId, H4504_CallHoldOperation::e_retrieveNotific);
  facilityPDU.BuildFacility(*this, TRUE);
  facilityPDU.AttachSupplementaryServiceAPDU(serviceAPDU);
  WriteSignalPDU(facilityPDU);
  
  // Pause channels
  for (PINDEX i = 0; i < logicalChannels->GetSize(); i++) {
    H323Channel * channel = logicalChannels->GetChannelAt(i);
    if (NULL != channel)
      channel->SetPause(false);
  }
  
  // Update hold state
  holdState = e_ch_Idle;
}


BOOL H323Connection::OnControlProtocolError(ControlProtocolErrors /*errorSource*/,
                                            const void * /*errorData*/)
{
  return TRUE;
}


void H323Connection::OnSendCapabilitySet(H245_TerminalCapabilitySet & /*pdu*/)
{
}


BOOL H323Connection::OnReceivedCapabilitySet(const H323Capabilities & remoteCaps,
                                             const H245_MultiplexCapability * muxCap,
                                             H245_TerminalCapabilitySetReject & /*rejectPDU*/)
{
  if (muxCap != NULL) {
    if (muxCap->GetTag() != H245_MultiplexCapability::e_h2250Capability) {
      PTRACE(1, "H323\tCapabilitySet contains unsupported multiplex.");
      return FALSE;
    }

    const H245_H2250Capability & h225_0 = *muxCap;
    remoteMaxAudioDelayJitter = h225_0.m_maximumAudioDelayJitter;
  }

  if (remoteCaps.GetSize() == 0) {
    // Received empty TCS, so close all transmit channels
    for (PINDEX i = 0; i < logicalChannels->GetSize(); i++) {
      H245NegLogicalChannel & negChannel = logicalChannels->GetNegLogicalChannelAt(i);
      H323Channel * channel = negChannel.GetChannel();
      if (channel != NULL && !channel->GetNumber().IsFromRemote())
        negChannel.Close();
    }
    transmitterSidePaused = TRUE;
  }
  else { // Received non-empty TCS
    if (transmitterSidePaused)
      remoteCapabilities.RemoveAll();

    if (!remoteCapabilities.Merge(remoteCaps))
      return FALSE;

    if (transmitterSidePaused) {
      transmitterSidePaused = FALSE;
      connectionState = HasExecutedSignalConnect;
      capabilityExchangeProcedure->Start(TRUE);
      masterSlaveDeterminationProcedure->Start(TRUE);
    }
    else
      capabilityExchangeProcedure->Start(FALSE);
  }

  return TRUE;
}


void H323Connection::SendCapabilitySet(BOOL empty)
{
  capabilityExchangeProcedure->Start(TRUE, empty);
}


void H323Connection::OnSetLocalCapabilities()
{
  OpalMediaFormatList formats = ownerCall.GetMediaFormats(*this);
  endpoint.AdjustMediaFormats(*this, formats);

  PINDEX simultaneous = P_MAX_INDEX;

  PINDEX i;
  for (i = 0; i < formats.GetSize(); i++) {
    if (formats[i].GetDefaultSessionID() == OpalMediaFormat::DefaultAudioSessionID)
      simultaneous = localCapabilities.AddAllCapabilities(endpoint, 0, simultaneous, formats[i]);
  }

  simultaneous = P_MAX_INDEX;

  for (i = 0; i < formats.GetSize(); i++) {
    if (formats[i].GetDefaultSessionID() == OpalMediaFormat::DefaultVideoSessionID)
      simultaneous = localCapabilities.AddAllCapabilities(endpoint, 0, simultaneous, formats[i]);
  }

  simultaneous = P_MAX_INDEX;

  for (i = 0; i < formats.GetSize(); i++) {
    if (formats[i].GetDefaultSessionID() == OpalMediaFormat::DefaultDataSessionID)
      simultaneous = localCapabilities.AddAllCapabilities(endpoint, 0, simultaneous, formats[i]);
  }

  PTRACE(2, "H323\tSetLocalCapabilities:\n" << setprecision(2) << localCapabilities);
}


BOOL H323Connection::IsH245Master() const
{
  return masterSlaveDeterminationProcedure->IsMaster();
}


void H323Connection::StartRoundTripDelay()
{
  if (Lock()) {
    if (connectionState == EstablishedConnection && !roundTripDelayTimer.IsRunning()) {
      roundTripDelayTimer = endpoint.GetRoundTripDelayRate();

      if (roundTripDelayProcedure->IsRemoteOffline()) {
        PTRACE(2, "H245\tRemote failed to respond to PDU.");
        if (endpoint.ShouldClearCallOnRoundTripFail())
          ClearCall(EndedByTransportFail);
      }
      else
        roundTripDelayProcedure->StartRequest();
    }
    Unlock();
  }
}


PTimeInterval H323Connection::GetRoundTripDelay() const
{
  return roundTripDelayProcedure->GetRoundTripDelay();
}


void H323Connection::InternalEstablishedConnectionCheck()
{
  PTRACE(3, "H323\tInternalEstablishedConnectionCheck: "
            "connectionState=" << connectionState << " "
            "fastStartState=" << fastStartState);

  BOOL h245_available = masterSlaveDeterminationProcedure->IsDetermined() &&
                        capabilityExchangeProcedure->HasSentCapabilities() &&
                        capabilityExchangeProcedure->HasReceivedCapabilities();

  // Check for if all the 245 conditions are met so can start up logical
  // channels and complete the connection establishment.
  if (fastStartState != FastStartAcknowledged) {
    if (!h245_available)
      return;

    // If we are early starting, start channels as soon as possible instead of
    // waiting for connect PDU
    if (earlyStart && FindChannel(OpalMediaFormat::DefaultAudioSessionID, FALSE) == NULL)
      OnSelectLogicalChannels();
  }

  if (h245_available && startT120) {
    if (remoteCapabilities.FindCapability("T.120") != NULL) {
      H323Capability * capability = localCapabilities.FindCapability("T.120");
      if (capability != NULL)
        OpenLogicalChannel(*capability, 3, H323Channel::IsBidirectional);
    }
    startT120 = FALSE;
  }

  if (connectionState != HasExecutedSignalConnect)
    return;

  // Check if we have already got a transmitter running, select one if not
  if (FindChannel(OpalMediaFormat::DefaultAudioSessionID, FALSE) == NULL)
    OnSelectLogicalChannels();

  connectionState = EstablishedConnection;
  connectionStartTime = PTime();
  OnEstablished();
}


OpalMediaFormatList H323Connection::GetMediaFormats() const
{
  return remoteCapabilities.GetMediaFormats();
}


BOOL H323Connection::OpenSourceMediaStream(const OpalMediaFormatList & mediaFormats,
                                           unsigned sessionID)
{
  // Check if we have already got a transmitter running, select one if not
  if (fastStartState != FastStartDisabled || FindChannel(sessionID, FALSE) != NULL)
    return FALSE;

  PStringArray order(mediaFormats.GetSize());
  for (PINDEX i = 0; i < mediaFormats.GetSize(); i++)
    order[i] = mediaFormats[i];
  localCapabilities.Reorder(order);
  OnSelectLogicalChannels();
  return TRUE;
}


OpalMediaStream * H323Connection::CreateMediaStream(BOOL isSource, unsigned sessionID)
{
  if (isSource || fastStartedTransmitMediaStream == NULL)
    return new OpalRTPMediaStream(isSource, *GetSession(sessionID));

  OpalMediaStream * stream = fastStartedTransmitMediaStream;
  fastStartedTransmitMediaStream = NULL;
  return stream;
}


void H323Connection::StartFastStartChannel(unsigned sessionID, H323Channel::Directions direction)
{
  for (PINDEX i = 0; i < fastStartChannels.GetSize(); i++) {
    H323Channel & channel = fastStartChannels[i];
    if (channel.GetSessionID() == sessionID && channel.GetDirection() == direction) {
      if (direction == H323Channel::IsTransmitter) {
        if (channel.Open()) {
          fastStartedTransmitMediaStream = ((H323UnidirectionalChannel &)channel).GetMediaStream();
          if (GetCall().OpenSourceMediaStreams(*this, fastStartedTransmitMediaStream->GetMediaFormats(), channel.GetSessionID()))
            channel.Start();
          else
            fastStartedTransmitMediaStream = NULL;
        }
      }
      else
        channel.Start();
      break;
    }
  }
}


void H323Connection::OnSelectLogicalChannels()
{
  PTRACE(2, "H245\tDefault OnSelectLogicalChannels, " << fastStartState);

  // Select the first codec that uses the "standard" audio session.
  switch (fastStartState) {
    default : //FastStartDisabled :
      SelectDefaultLogicalChannel(OpalMediaFormat::DefaultAudioSessionID);
      if (endpoint.CanAutoStartTransmitVideo())
        SelectDefaultLogicalChannel(OpalMediaFormat::DefaultVideoSessionID);
      break;

    case FastStartInitiate :
      SelectFastStartChannels(OpalMediaFormat::DefaultAudioSessionID, TRUE, TRUE);
      SelectFastStartChannels(OpalMediaFormat::DefaultVideoSessionID,
                              endpoint.CanAutoStartTransmitVideo(),
                              endpoint.CanAutoStartReceiveVideo());
      break;

    case FastStartResponse :
      StartFastStartChannel(OpalMediaFormat::DefaultAudioSessionID, H323Channel::IsTransmitter);
      StartFastStartChannel(OpalMediaFormat::DefaultAudioSessionID, H323Channel::IsReceiver);
      if (endpoint.CanAutoStartTransmitVideo())
        StartFastStartChannel(OpalMediaFormat::DefaultVideoSessionID, H323Channel::IsTransmitter);
      if (endpoint.CanAutoStartReceiveVideo())
        StartFastStartChannel(OpalMediaFormat::DefaultVideoSessionID, H323Channel::IsReceiver);
      break;
  }
}


void H323Connection::SelectDefaultLogicalChannel(unsigned sessionID)
{
  for (PINDEX i = 0; i < localCapabilities.GetSize(); i++) {
    H323Capability & localCapability = localCapabilities[i];
    if (localCapability.GetDefaultSessionID() == sessionID) {
      H323Capability * remoteCapability = remoteCapabilities.FindCapability(localCapability);
      if (remoteCapability != NULL) {
        PTRACE(3, "H323\tSelecting " << *remoteCapability);
        if (OpenLogicalChannel(*remoteCapability, sessionID, H323Channel::IsTransmitter))
          break;
        PTRACE(2, "H323\tOnSelectLogicalChannels, OpenLogicalChannel failed: "
               << *remoteCapability);
      }
    }
  }
}


void H323Connection::SelectFastStartChannels(unsigned sessionID,
                                             BOOL transmitter,
                                             BOOL receiver)
{
  // Select all of the fast start channels to offer to the remote when initiating a call.
  for (PINDEX i = 0; i < localCapabilities.GetSize(); i++) {
    H323Capability & capability = localCapabilities[i];
    if (capability.GetDefaultSessionID() == sessionID) {
      if (receiver) {
        if (!OpenLogicalChannel(capability, sessionID, H323Channel::IsReceiver)) {
          PTRACE(2, "H323\tOnSelectLogicalChannels, OpenLogicalChannel rx failed: " << capability);
        }
      }
      if (transmitter) {
        if (!OpenLogicalChannel(capability, sessionID, H323Channel::IsTransmitter)) {
          PTRACE(2, "H323\tOnSelectLogicalChannels, OpenLogicalChannel tx failed: " << capability);
        }
      }
    }
  }
}


BOOL H323Connection::OpenLogicalChannel(const H323Capability & capability,
                                        unsigned sessionID,
                                        H323Channel::Directions dir)
{
  switch (fastStartState) {
    default : // FastStartDisabled
      if (dir == H323Channel::IsReceiver)
        return FALSE;

      // Traditional H245 handshake
      return logicalChannels->Open(capability, sessionID);

    case FastStartResponse :
      // Do not use OpenLogicalChannel for starting these.
      return FALSE;

    case FastStartInitiate :
      break;
  }

  /*If starting a receiver channel and are initiating the fast start call,
    indicated by the remoteCapabilities being empty, we do a "trial"
    listen on the channel. That is, for example, the UDP sockets are created
    to receive data in the RTP session, but no thread is started to read the
    packets and pass them to the codec. This is because at this point in time,
    we do not know which of the codecs is to be used, and more than one thread
    cannot read from the RTP ports at the same time.
  */
  H323Channel * channel = capability.CreateChannel(*this, dir, sessionID, NULL);
  if (channel == NULL)
    return FALSE;

  if (dir != H323Channel::IsReceiver)
    channel->SetNumber(logicalChannels->GetNextChannelNumber());

  fastStartChannels.Append(channel);
  return TRUE;
}


BOOL H323Connection::OnOpenLogicalChannel(const H245_OpenLogicalChannel & /*openPDU*/,
                                          H245_OpenLogicalChannelAck & /*ackPDU*/,
                                          unsigned & /*errorCode*/)
{
  // If get a OLC via H.245 stop trying to do fast start
  fastStartState = FastStartDisabled;
  if (!fastStartChannels.IsEmpty()) {
    fastStartChannels.RemoveAll();
    PTRACE(1, "H245\tReceived early start OLC, aborting fast start");
  }

  //errorCode = H245_OpenLogicalChannelReject_cause::e_unspecified;
  return TRUE;
}


BOOL H323Connection::OnConflictingLogicalChannel(H323Channel & conflictingChannel)
{
  unsigned session = conflictingChannel.GetSessionID();
  PTRACE(2, "H323\tLogical channel " << conflictingChannel
         << " conflict on session " << session
         << ", codec: " << conflictingChannel.GetCapability());

  /* Matrix of conflicts:
       Local EP is master and conflicting channel from remote (OLC)
          Reject remote transmitter (function is not called)
       Local EP is master and conflicting channel to remote (OLCAck)
          Should not happen (function is not called)
       Local EP is slave and conflicting channel from remote (OLC)
          Close sessions reverse channel from remote
          Start new reverse channel using codec in conflicting channel
          Accept the OLC for masters transmitter
       Local EP is slave and conflicting channel to remote (OLCRej)
          Start transmitter channel using codec in sessions reverse channel

      Upshot is this is only called if a slave and require a restart of
      some channel. Possibly closing channels as master has precedence.
   */

  BOOL fromRemote = conflictingChannel.GetNumber().IsFromRemote();
  H323Channel * channel = FindChannel(session, !fromRemote);
  if (channel == NULL) {
    PTRACE(1, "H323\tCould not resolve conflict, no reverse channel.");
    return FALSE;
  }

  if (!fromRemote) {
    conflictingChannel.CleanUpOnTermination();
    H323Capability * capability = remoteCapabilities.FindCapability(channel->GetCapability());
    if (capability == NULL) {
      PTRACE(1, "H323\tCould not resolve conflict, capability not available on remote.");
      return FALSE;
    }
    OpenLogicalChannel(*capability, session, H323Channel::IsTransmitter);
    return TRUE;
  }

  // Close the conflicting channel that got in before our transmitter
  channel->CleanUpOnTermination();
  CloseLogicalChannelNumber(channel->GetNumber());

  // Must be slave and conflict from something we are sending, so try starting a
  // new channel using the master endpoints transmitter codec.
  OpenLogicalChannel(conflictingChannel.GetCapability(), session, H323Channel::IsTransmitter);
  return TRUE;
}


H323Channel * H323Connection::CreateLogicalChannel(const H245_OpenLogicalChannel & open,
                                                   BOOL startingFast,
                                                   unsigned & errorCode)
{
  const H245_H2250LogicalChannelParameters * param;
  const H245_DataType * dataType;
  H323Channel::Directions direction;

  if (startingFast && open.HasOptionalField(H245_OpenLogicalChannel::e_reverseLogicalChannelParameters)) {
    if (open.m_reverseLogicalChannelParameters.m_multiplexParameters.GetTag() !=
              H245_OpenLogicalChannel_reverseLogicalChannelParameters_multiplexParameters
                                                      ::e_h2250LogicalChannelParameters) {
      errorCode = H245_OpenLogicalChannelReject_cause::e_unsuitableReverseParameters;
      PTRACE(2, "H323\tCreateLogicalChannel - reverse channel, H225.0 only supported");
      return NULL;
    }

    PTRACE(3, "H323\tCreateLogicalChannel - reverse channel");
    dataType = &open.m_reverseLogicalChannelParameters.m_dataType;
    param = &(const H245_H2250LogicalChannelParameters &)
                      open.m_reverseLogicalChannelParameters.m_multiplexParameters;
    direction = H323Channel::IsTransmitter;
  }
  else {
    if (open.m_forwardLogicalChannelParameters.m_multiplexParameters.GetTag() !=
              H245_OpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters
                                                      ::e_h2250LogicalChannelParameters) {
      PTRACE(2, "H323\tCreateLogicalChannel - forward channel, H225.0 only supported");
      errorCode = H245_OpenLogicalChannelReject_cause::e_unspecified;
      return NULL;
    }

    PTRACE(3, "H323\tCreateLogicalChannel - forward channel");
    dataType = &open.m_forwardLogicalChannelParameters.m_dataType;
    param = &(const H245_H2250LogicalChannelParameters &)
                      open.m_forwardLogicalChannelParameters.m_multiplexParameters;
    direction = H323Channel::IsReceiver;
  }

  // See if datatype is supported
  H323Capability * capability = localCapabilities.FindCapability(*dataType);
  if (capability == NULL) {
    errorCode = H245_OpenLogicalChannelReject_cause::e_unknownDataType;
    PTRACE(2, "H323\tCreateLogicalChannel - unknown data type");
    return NULL; // If codec not supported, return error
  }

  if (!capability->OnReceivedPDU(*dataType, direction == H323Channel::IsReceiver)) {
    errorCode = H245_OpenLogicalChannelReject_cause::e_dataTypeNotSupported;
    PTRACE(2, "H323\tCreateLogicalChannel - data type not supported");
    return NULL; // If codec not supported, return error
  }

  if (startingFast && (direction == H323Channel::IsTransmitter))
    remoteCapabilities.SetCapability(0, 0, remoteCapabilities.Copy(*capability));

  if (!OnCreateLogicalChannel(*capability, direction, errorCode))
    return NULL; // If codec combination not supported, return error

  H323Channel * channel = capability->CreateChannel(*this, direction, 0, param);
  if (channel == NULL) {
    errorCode = H245_OpenLogicalChannelReject_cause::e_dataTypeNotAvailable;
    PTRACE(2, "H323\tCreateLogicalChannel - data type not available");
    return NULL;
  }

  if (!channel->SetInitialBandwidth())
    errorCode = H245_OpenLogicalChannelReject_cause::e_insufficientBandwidth;
  else if (channel->OnReceivedPDU(open, errorCode))
    return channel;

  PTRACE(2, "H323\tCreateLogicalChannel - insufficient bandwidth");
  delete channel;
  return NULL;
}


BOOL H323Connection::OnCreateLogicalChannel(const H323Capability & capability,
                                            H323Channel::Directions dir,
                                            unsigned & errorCode)
{
  if (connectionState == ShuttingDownConnection) {
    errorCode = H245_OpenLogicalChannelReject_cause::e_unspecified;
    return FALSE;
  }

  // Default error if returns FALSE
  errorCode = H245_OpenLogicalChannelReject_cause::e_dataTypeALCombinationNotSupported;

  // Check if in set at all
  if (dir != H323Channel::IsReceiver) {
    if (!remoteCapabilities.IsAllowed(capability)) {
      PTRACE(2, "H323\tOnCreateLogicalChannel - transmit capability " << capability << " not allowed.");
      return FALSE;
    }
  }
  else {
    if (!localCapabilities.IsAllowed(capability)) {
      PTRACE(2, "H323\tOnCreateLogicalChannel - receive capability " << capability << " not allowed.");
      return FALSE;
    }
  }

  // Check all running channels, and if new one can't run with it return FALSE
  for (PINDEX i = 0; i < logicalChannels->GetSize(); i++) {
    H323Channel * channel = logicalChannels->GetChannelAt(i);
    if (channel != NULL && channel->GetDirection() == dir) {
      if (dir != H323Channel::IsReceiver) {
        if (!remoteCapabilities.IsAllowed(capability, channel->GetCapability())) {
          PTRACE(2, "H323\tOnCreateLogicalChannel - transmit capability " << capability
                 << " and " << channel->GetCapability() << " incompatible.");
          return FALSE;
        }
      }
      else {
        if (!localCapabilities.IsAllowed(capability, channel->GetCapability())) {
          PTRACE(2, "H323\tOnCreateLogicalChannel - transmit capability " << capability
                 << " and " << channel->GetCapability() << " incompatible.");
          return FALSE;
        }
      }
    }
  }

  return TRUE;
}


BOOL H323Connection::OnStartLogicalChannel(H323Channel & channel)
{
  return endpoint.OnStartLogicalChannel(*this, channel);
}


void H323Connection::CloseLogicalChannel(unsigned number, BOOL fromRemote)
{
  if (connectionState != ShuttingDownConnection)
    logicalChannels->Close(number, fromRemote);
}


void H323Connection::CloseLogicalChannelNumber(const H323ChannelNumber & number)
{
  CloseLogicalChannel(number, number.IsFromRemote());
}


BOOL H323Connection::OnClosingLogicalChannel(H323Channel & /*channel*/)
{
  return TRUE;
}


void H323Connection::OnClosedLogicalChannel(const H323Channel & channel)
{
  mediaStreams.Remove(channel.GetMediaStream());
  endpoint.OnClosedLogicalChannel(*this, channel);
}


void H323Connection::OnLogicalChannelFlowControl(H323Channel * channel,
                                                 long bitRateRestriction)
{
  if (channel != NULL)
    channel->OnFlowControl(bitRateRestriction);
}


void H323Connection::OnLogicalChannelJitter(H323Channel * channel,
                                            DWORD jitter,
                                            int skippedFrameCount,
                                            int additionalBuffer)
{
  if (channel != NULL)
    channel->OnJitterIndication(jitter, skippedFrameCount, additionalBuffer);
}


unsigned H323Connection::GetBandwidthUsed() const
{
  unsigned used = 0;

  for (PINDEX i = 0; i < logicalChannels->GetSize(); i++) {
    H323Channel * channel = logicalChannels->GetChannelAt(i);
    if (channel != NULL)
      used += channel->GetBandwidthUsed();
  }

  PTRACE(3, "H323\tBandwidth used: " << used);

  return used;
}


BOOL H323Connection::UseBandwidth(unsigned bandwidth, BOOL removing)
{
  PTRACE(3, "H323\tBandwidth request: "
         << (removing ? '+' : '-')
         << bandwidth/10 << '.' << bandwidth%10
         << "kb/s, available: "
         << bandwidthAvailable/10 << '.' << bandwidthAvailable%10
         << "kb/s");

  if (removing)
    bandwidthAvailable += bandwidth;
  else {
    if (bandwidth > bandwidthAvailable) {
      PTRACE(2, "H323\tAvailable bandwidth exceeded");
      return FALSE;
    }

    bandwidthAvailable -= bandwidth;
  }

  return TRUE;
}


BOOL H323Connection::SetBandwidthAvailable(unsigned newBandwidth, BOOL force)
{
  unsigned used = GetBandwidthUsed();
  if (used > newBandwidth) {
    if (!force)
      return FALSE;

    // Go through logical channels and close down some.
    PINDEX chanIdx = logicalChannels->GetSize();
    while (used > newBandwidth && chanIdx-- > 0) {
      H323Channel * channel = logicalChannels->GetChannelAt(chanIdx);
      if (channel != NULL) {
        used -= channel->GetBandwidthUsed();
        CloseLogicalChannelNumber(channel->GetNumber());
      }
    }
  }

  bandwidthAvailable = newBandwidth - used;
  return TRUE;
}


void H323Connection::SendUserInputIndication(const H245_UserInputIndication & indication)
{
  H323ControlPDU pdu;
  H245_UserInputIndication & ind = pdu.Build(H245_IndicationMessage::e_userInput);
  ind = indication;
  WriteControlPDU(pdu);
}


void H323Connection::SendUserInput(const PString & value)
{
  PTRACE(2, "H323\tSendUserInput(\"" << value << "\")");
  H323ControlPDU pdu;
  PASN_GeneralString & str = pdu.BuildUserInputIndication(value);
  if (!str.GetValue())
    WriteControlPDU(pdu);
}


void H323Connection::SendUserInputTone(char tone,
                                       unsigned duration,
                                       unsigned logicalChannel,
                                       unsigned rtpTimestamp)
{
  PTRACE(2, "H323\tSendUserInputTone("
         << tone << ','
         << duration << ','
         << logicalChannel << ','
         << rtpTimestamp << ')');
  H323ControlPDU pdu;
  pdu.BuildUserInputIndication(tone, duration, logicalChannel, rtpTimestamp);
  WriteControlPDU(pdu);
}


void H323Connection::OnUserInputIndication(const H245_UserInputIndication & ind)
{
  switch (ind.GetTag()) {
    case H245_UserInputIndication::e_alphanumeric :
      OnUserInputString((const PASN_GeneralString &)ind);
      break;
    case H245_UserInputIndication::e_signal :
      const H245_UserInputIndication_signal & sig = ind;
      OnUserInputTone(sig.m_signalType[0],
                      sig.m_duration,
                      sig.m_rtp.m_logicalChannelNumber,
                      sig.m_rtp.m_timestamp);
  }
}


void H323Connection::OnUserInputString(const PString & value)
{
  endpoint.OnUserInputString(*this, value);
}


void H323Connection::OnUserInputTone(char tone,
                                     unsigned duration,
                                     unsigned logicalChannel,
                                     unsigned rtpTimestamp)
{
  endpoint.OnUserInputTone(*this, tone, duration, logicalChannel, rtpTimestamp);
}


RTP_Session * H323Connection::GetSession(unsigned sessionID) const
{
  return rtpSessions.GetSession(sessionID);
}


H323_RTP_Session * H323Connection::GetSessionCallbacks(unsigned sessionID) const
{
  RTP_Session * session = rtpSessions.GetSession(sessionID);
  if (session == NULL)
    return NULL;

  PTRACE(3, "RTP\tFound existing session " << sessionID);
  PObject * data = session->GetUserData();
  PAssert(data->IsDescendant(H323_RTP_Session::Class()), PInvalidCast);
  return (H323_RTP_Session *)data;
}


RTP_Session * H323Connection::UseSession(unsigned sessionID,
                                         const H245_TransportAddress & taddr)
{
  // We only support unicast IP at this time.
  if (taddr.GetTag() != H245_TransportAddress::e_unicastAddress)
    return NULL;

  const H245_UnicastAddress & uaddr = taddr;
  if (uaddr.GetTag() != H245_UnicastAddress::e_iPAddress)
    return NULL;

  RTP_Session * session = rtpSessions.UseSession(sessionID);
  if (session != NULL)
    return session;

  RTP_UDP * udp_session = new RTP_UDP(sessionID);
  udp_session->SetUserData(new H323_RTP_UDP(*this, *udp_session));
  rtpSessions.AddSession(udp_session);
  return udp_session;
}


void H323Connection::ReleaseSession(unsigned sessionID)
{
  rtpSessions.ReleaseSession(sessionID);
}


void H323Connection::OnRTPStatistics(const RTP_Session & session) const
{
  endpoint.OnRTPStatistics(*this, session);
}


PString H323Connection::GetSessionCodecNames(unsigned sessionID) const
{
  PStringStream name;

  H323Channel * channel = FindChannel(sessionID, FALSE);
  if (channel != NULL)
    name << *channel;

  return name;
}


void H323Connection::RequestModeChange()
{
}


BOOL H323Connection::OnRequestModeChange(const H245_RequestMode & /*pdu*/,
                                         H245_RequestModeAck & /*ack*/,
                                         H245_RequestModeReject & /*reject*/)
{
  return TRUE;
}


void H323Connection::OnAcceptModeChange(const H245_RequestModeAck & /*pdu*/)
{
}


void H323Connection::OnRefusedModeChange(const H245_RequestModeReject * /*pdu*/)
{
}


const OpalTransport & H323Connection::GetControlChannel() const
{
  return *(controlChannel != NULL ? controlChannel : signallingChannel);
}


/////////////////////////////////////////////////////////////////////////////
