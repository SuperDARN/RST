;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
;	NAME:
; 	acf_plot_frontpage
;
; PURPOSE:
; 	plots RTP slices for a given beam-sounding for the frontpage of RAWACF plots
;
; CATEGORY:
; 	Graphics
;
; CALLING SEQUENCE:
;		acf_plot_frontpage,yr,mo,dy,hr,mt,sc,radarname,bmnum,tfreq,nave,cpid,nrang,$
;												noise,fitmeth,qflg,pow,vel,wid,ngood,lat,lagfr,smsep
;
;
;	INPUTS:
;		yr: 				year
;		mo: 				month
;		dy:					day
;		hr:					hour
;		mt:					minute
;		sc:					second
;		radarname:	name of the radar
;		bmnum:			beam number
;		tfreq:			transmit frequency
;		nave:				number of averages
;		cpid:				control program id number
;		nrang:			number of range gates
;		noise:			noise level
;		fitmeth:		fitting method used in processing (0=FITACF,1=FITEX2,2=LMFIT)
;		qflg:				array of size nrang containing qflgs for the range gates
;		pow:				array of size nrang containing the powers for the range gates
;		vel:				array of size nrang containing the velocities for the range gates
;		wid:				array of size nrang containing the spectral widths for the range gates
;		lat:				latitude of the radar
;		lagfr:			lag to first range (in us)
;		smsep:			sample separation (in us)
;
; OPTIONAL INPUTS:
;
; KEYWORD PARAMETERS:
;
; EXAMPLE:
;
; OUTPUT:
;
;
; COPYRIGHT:
; Copyright (C) 2011 by Virginia Tech
;
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in
; all copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
; THE SOFTWARE.
;
;
; MODIFICATION HISTORY:
; Written by AJ Ribeiro 02/06/2012
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

pro acf_plot_frontpage,yr,mo,dy,hr,mt,sc,radarname,bmnum,tfreq,nave,cpid,nrang,$
										noise,fitmeth,qflg,pow,vel,wid,ngood,lat,lagfr,smsep

	date_str = radarname+'	 '+$
							strtrim(fix(yr),2)+'/'+strtrim(fix(mo),2)+'/'+strtrim(fix(dy),2)
  date_str = date_str+'	 '
  date_str = date_str+strtrim(fix(hr),2)+':'+strtrim(fix(mt),2)+':'+strtrim(fix(sc),2)+' UT'
  date_str = date_str+'		'
  date_str = date_str+'Beam: '+strtrim(fix(bmnum),2)
    date_str = date_str+'		'
  date_str = date_str+'Freq: '+strtrim(fix(tfreq),2)+' kHz'
  date_str = date_str + '  Nrang: '+strtrim(round(nrang),2)

  xyouts,.5,.97,date_str,align=.5,charsize=.8,charthick=3.,/normal

  mystr = 'Nave: '+strtrim(round(nave),2)
  mystr = mystr + '  CPID: '+strtrim(round(cpid),2)+' ('+get_cp_name(round(cpid))+')'
  mystr = mystr + '  Noise: '+strtrim(round(noise),2)
  mystr = mystr + '  Lagfr: '+strtrim(round(lagfr),2)+' us'
  mystr = mystr + '  Smsep: '+strtrim(round(smsep),2)+' us'

  xyouts,.5,.93,mystr,align=.5,charsize=.8,charthick=3.,/normal


  case fitmeth of
		0: xyouts,.5,.89,'FITACF results',align=.5,charsize=.8,charthick=3.,/normal
		1: xyouts,.5,.89,'FITEX2 results',align=.5,charsize=.8,charthick=3.,/normal
		else: xyouts,.5,.89,'LMFIT results',align=.5,charsize=.8,charthick=3.,/normal
	endcase

	titles=['Power','Velocity','Width','Good Lags']
	units=['dB','m/s','m/s','#']
	pos0 = [.15,.05,.19,.84]
	ytitles=['Range Gate','','','']
	scales = [[0,-1000,0,0],[30,1000,200,20]]
	params=[[pow],[vel],[wid],[ngood]]
	param_str=['power','velocity','width','width']
	if(lat lt 48) then begin
		scales(1,*) = [-200,200]
		scales(2,*) = [0,75]
	endif

	;draw the colorbars first
	for i=0,3 do begin
			mypos = pos0 + [i*.2,0,i*.2,0]
			if(i lt 3) then $
			plot_colorbar,1,1,0,0,pos=[mypos(0)+.08,.4,mypos(2)+.06,.7],param=param_str(i),scale=[scales(i,0),scales(i,1)], $
			charsize=.6 $
		else $
			plot_colorbar,1,1,0,0,pos=[mypos(0)+.08,.4,mypos(2)+.06,.7],param=param_str(i),scale=[scales(i,0),scales(i,1)], $
			legend='Good Lags [#]',charsize=.6
	endfor

	;now draw the plots
	for i=0,3 do begin
		mypos = pos0 + [i*.2,0,i*.2,0]
		plot,findgen(1),findgen(nrang),xrange=[0,1],yrange=[0,nrang],/nodata,/noerase,pos=mypos,$
				ytitle=ytitles(i),xthick=3.,ythick=3.,charthick=3.,xticks=1,xtickname=replicate(' ',10),$
				xstyle=1,ystyle=1,title=titles(i),charsize=.7,yticklen=-.15

		for j=0,nrang-1 do begin
			if(qflg(j) ne 1) then continue
			color = get_color_index(params(j,i),param=param_str(i),scale=[scales(i,0),scales(i,1)])
			polyfill,[0,1,1,0],[j,j,j+1,j+1],col=color,/data
		endfor

		for j=0,nrang-1 do plots,[0,1],[j,j],/data,thick=2.

	endfor


	erase
end