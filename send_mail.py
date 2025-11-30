import os

import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email import policy

from dotenv import load_dotenv

load_dotenv()

def create_message(to_email: str, subject: str, body: str) -> MIMEMultipart:
    """
    multipart形式のMIMEメッセージを作成
    Args:
        to_email (str): 送信先メールアドレス
        subject (str): メールの件名
        body (str): メールの本文
    Returns:
        MIMEMultipart: 作成したメールメッセージ
    """
    msg = MIMEMultipart('alternative', policy=policy.SMTP)

    # メッセージを作成
    msg['Subject'] = subject
    msg['From'] = os.getenv('MY_MAIL_ADDRESS')
    msg['To'] = to_email
    msg.attach(MIMEText(body, 'plain', "utf-8"))
    return msg


def send_email(msg: MIMEMultipart):
    """
    SMTPサーバーを使用してメールを送信
    Args:
        msg (MIMEMultipart): 送信するメールメッセージ
    """
    # smtplib.SMTP_SSLではなくsmtplib.SMTPを利用
    with smtplib.SMTP(os.getenv('SMTP_SERVER'), os.getenv('SMTP_PORT')) as server:
        server.starttls()  # 追加
        server.login(os.getenv('MY_MAIL_ADDRESS'), os.getenv('APP_PASS'))
        server.send_message(msg)

def prosess_mail(text="いや、なにもない"):
    to_email = os.getenv("MY_MAIL_ADDRESS")
    subject = '実行中のPythonからのメール'
    body = f"上手くいけたぞぞぞぞ、\n{text}"

    # メッセージの作成
    message = create_message(
        to_email=to_email,
        subject=subject,
        body=body
    )
    # メールの送信
    send_email(message)



if __name__ == '__main__':
    to_email = os.getenv("MY_MAIL_ADDRESS")
    subject = '実行中のPythonからのメール'
    body = 'このメールはPythonから自動送信しています。'

    # メッセージの作成
    message = create_message(
        to_email=to_email,
        subject=subject,
        body=body
    )
    # メールの送信
    send_email(message)
