# 2주차

## C++ 를 이용해 논블로킹 + 싱크 방식을 이용한 간단한 웹 백엔드 만들기

## 기능 설명


### 논블록 방식으로 요청 받기
- 무한 루프를 돌면서 요청을 계송 받는다.
- accept() 함수가 븡로킹 되지 않는다.

<br>


### 논블록 방식으로 요청 받기
- recv() 를 통해 데이터를 받는다.
- 무한 루프를 돌면서 데이터가 도착하지 않거나 블로킹 되지 않은 경우 계속 대기한다
- 데이터가 정상적으로 수신될 경우 루프를 종료하곧 ㅏ음으로 넘아간다.

<img width="573" alt="스크린샷 2024-02-19 12 18 04" src="https://github.com/OppSpark/2024_network_study/assets/137988657/3e92c84b-c118-4005-98bd-dad5fe999a61">

<br>



## HTTP 프로토콜 규약에 맞춰 LOW-단에서 HTML 라우팅하기




### GET 로 HTML 라우팅하기
<br>

- 메인 페이지 라우팅
<img width="598" alt="스크린샷 2024-02-19 12 14 03" src="https://github.com/OppSpark/2024_network_study/assets/137988657/64a65bb8-3cbf-4bd7-ac99-7872c0d64b56">

<br>

### 쿠키 삽입 및 삭제
<br>

- 로그인 기능 및 사원 조획 기능
- 쿠키넣기, 삭제
-  set-cookie : id =  ...
<img width="946" alt="스크린샷 2024-02-19 12 12 10" src="https://github.com/OppSpark/2024_network_study/assets/137988657/0042f89d-7c9b-4a64-8bf1-c6ccd7748cdf">

<br>
<br>


- 로그인 버튼을 누르면 브랑우저에 에 쿠키가 들어간다
- 로그아웃 버튼을 누르면 쿠기가 삭제 된다.

<img width="724" alt="스크린샷 2024-02-19 12 14 25" src="https://github.com/OppSpark/2024_network_study/assets/137988657/990f634c-a4d5-4153-936f-84937df30ab9">
<br>
<br>


- ### 쿠키가 있을 수 도 있고, 없을 수 도 있고
<br>

- 쿠키가 있을 때 사원 목록을 json 형태로 출력
<img width="1188" alt="스크린샷 2024-02-19 12 15 07" src="https://github.com/OppSpark/2024_network_study/assets/137988657/0500b110-029e-4629-a152-19cf683ad917">
<br>
<br>


- 쿠키가 없을 때 에러 페이지를 json 형태로 출력
<img width="565" alt="스크린샷 2024-02-19 12 14 57" src="https://github.com/OppSpark/2024_network_study/assets/137988657/2a0d51f4-3566-4cce-80b9-7ed1a5b1e3e4">
<br>
<br>


### POST 받기
<br>

- 해당 URL로 POST를 보내면 output.txt에 내용을 저장하고 응답을 준다.
<img width="822" alt="스크린샷 2024-02-19 12 30 48" src="https://github.com/OppSpark/2024_network_study/assets/137988657/90064a18-c747-4162-93d0-5d6f547e97dc">





  


